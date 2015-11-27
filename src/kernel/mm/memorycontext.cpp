/***************************************************************************
 *   Copyright 2015 by Davide Bettio <davide.bettio@kdemail.net>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************
 *   Name: memorycontext.h                                                 *
 *   Date: 20/11/2015                                                      *
 ***************************************************************************/

#include <mm/memorycontext.h>

#include <stdint.h>
#include <cstdlib.h>
#include <gccbuiltins.h>
#include <core/printk.h>
#include <filesystem/vnodemanager.h>
#include <task/scheduler.h>

#ifdef ARCH_IA32_NATIVE
#include <arch/ia32/mm/pagingmanager.h>
#endif

const char *memoryOperationToString(UserspaceMemoryManager::MemoryOperation op)
{
    switch (op) {
        case UserspaceMemoryManager::ReadOperation:
            return "read";
        break;

        case UserspaceMemoryManager::WriteOperation:
            return "write";
        break;

        case UserspaceMemoryManager::ExecuteOperation:
            return "execute";
        break;

        default:
            return "access";
        break;
     }
}

void segmentationFault(void *faultAddress, void *faultPC, UserspaceMemoryManager::MemoryOperation op, UserspaceMemoryManager::PageFaultFlags flags, const char *memoryDescriptorTypeString)
{
    int pid = 0;
    if (LIKELY(Scheduler::currentThread() && Scheduler::currentThread()->parentProcess)) {
        pid = Scheduler::currentThread()->parentProcess->pid;
    }

    printk("[Process %i] Segmentation Fault: instruction at 0x%p tried to %s %s memory address 0x%p.\n", pid, faultPC, memoryOperationToString(op), memoryDescriptorTypeString, faultAddress);
    while(1);
}

MemoryContext::MemoryContext()
{
    m_descriptors = new QList<MemoryDescriptor *>();
    m_vmemAlloc.init((1 << 29) - (2 << 10));
}

inline unsigned long roundToPageMultiples(unsigned long l)
{
    return ((l & 0xFFFFF000) + ((l & 0xFFF) ? 0x1000 : 0));
}

MemoryDescriptor *MemoryContext::findMemoryDescriptor(void *address) const
{
    for (int i = 0; i < m_descriptors->count(); i++) {
        MemoryDescriptor *d = m_descriptors->at(i);
//        printk("checking: 0x%x - 0x%x\n", d->baseAddress, ((unsigned long) d->baseAddress) + ((d->length & 0xFFFFF000) + 0x1000));
        if (((unsigned long) address >= (unsigned long) d->baseAddress) &&
            ((unsigned long) address < ((unsigned long) d->baseAddress) + roundToPageMultiples(d->length))) {
            return d;
        }
    }

    return NULL;
}

QList<MemoryDescriptor *> *MemoryContext::findMemoryDescriptorsByRange(void *low, void *hi)
{
//    printk("range: %p - %p\n", low, hi);
    QList<MemoryDescriptor *> *foundDescriptors = new QList<MemoryDescriptor *>();

    for (int i = 0; i < m_descriptors->count(); i++) {
        MemoryDescriptor *d = m_descriptors->at(i);
//        printk("checking: 0x%x - 0x%x\n", d->baseAddress, ((unsigned long) d->baseAddress) + roundToPageMultiples(d->length));
        if (
            ( ((unsigned long) low) <= (((unsigned long) d->baseAddress) + roundToPageMultiples(d->length)) ) &&
            ( ((unsigned long) hi) >= (((unsigned long) d->baseAddress) + roundToPageMultiples(d->length)) )
           ) {
//        printk("found: 0x%x - 0x%x\n", d->baseAddress, ((unsigned long) d->baseAddress) + roundToPageMultiples(d->length));
            foundDescriptors->append(d);
        }
    }

    return foundDescriptors;

}

void MemoryContext::handlePageFault(void *faultAddress, void *faultPC, UserspaceMemoryManager::MemoryOperation op, UserspaceMemoryManager::PageFaultFlags flags)
{
//    printk("Fault address: 0x%p\n", faultAddress);
    MemoryDescriptor *mDesc = findMemoryDescriptor(faultAddress);

    if (UNLIKELY(!mDesc)) {
        //Unmapped space, we are not allowed to do anything here, just segfault
        segmentationFault(faultAddress, faultPC, op, flags, "unmapped");
        return;
    }

    switch (op) {
        case UserspaceMemoryManager::WriteOperation:
            if (UNLIKELY(!(mDesc->permissions & MemoryDescriptor::WritePermission))) {
                segmentationFault(faultAddress, faultPC, op, flags, "not writeable");
                return;
            }
        break;

        case UserspaceMemoryManager::ReadOperation:
            if (UNLIKELY(!(mDesc->permissions & MemoryDescriptor::ReadPermission))) {
                segmentationFault(faultAddress, faultPC, op, flags, "not readable");
                return;
            }
        break;

        case UserspaceMemoryManager::ExecuteOperation:
            if (UNLIKELY(!(mDesc->permissions & MemoryDescriptor::ExecutePermission))) {
                segmentationFault(faultAddress, faultPC, op, flags, "not executable");
                return;
            }
        break;
    }

    if ((flags & UserspaceMemoryManager::MissingPageFault) && (mDesc->flags == MemoryDescriptor::AnonymousMemory)) {
        #ifdef ARCH_IA32_NATIVE
            unsigned long npFlags = (mDesc->permissions & MemoryDescriptor::WritePermission) ? PagingManager::Write : 0;
            PagingManager::newPage((uint32_t) faultAddress, npFlags);
        #endif

    } else if (flags & UserspaceMemoryManager::MissingPageFault && (mDesc->flags == MemoryDescriptor::MemoryMappedFile)) {
       MemoryMappedFileDescriptor *mfDesc = (MemoryMappedFileDescriptor *) mDesc;
       #ifdef ARCH_IA32_NATIVE
           unsigned long npFlags = (mfDesc->permissions & MemoryDescriptor::WritePermission) ? PagingManager::Write : 0;
           PagingManager::newPage((uint32_t) faultAddress, npFlags);
       #endif
       unsigned long virtualPageAddress = (((unsigned long) faultAddress) & 0xFFFFF000);
       unsigned long offset = virtualPageAddress - ((unsigned long) mfDesc->baseAddress);
       int res = FS_CALL(mfDesc->node, read)(mfDesc->node, offset, (char *) virtualPageAddress, 4096);
       if (res < 0) {
           printk("I/O error while loading page (read bytes: %i)\n", res);
           segmentationFault(faultAddress, faultPC, op, flags, "missing page");
       }
   } else {
       printk("error: unexpected memory mapping type\n");
       segmentationFault(faultAddress, faultPC, op, flags, "unmanaged");
   }
}

int MemoryContext::insertMemoryDescriptor(MemoryDescriptor *descriptor)
{
    m_descriptors->append(descriptor);
    return 0;
}

void *MemoryContext::findEmptyMemoryExtent(void *baseAddress, unsigned long length, MemoryContext::MemoryAllocationHints hints)
{
    void *newAddress = baseAddress;

#if 0
    if (baseAddress) {
        // TODO: make sure we are not trying to allocate overlapping memory regions

    }
#endif

    if (baseAddress == NULL) {
        unsigned long pageIndex = m_vmemAlloc.allocateBlocks((length >> 12) + ((length & 0xFFF) != 0));
        newAddress = (void *) ((1 << 29) + (pageIndex << 12));
    }

    return newAddress;
}

int MemoryContext::allocateAnonymousMemory(void **baseAddress, unsigned long length, MemoryDescriptor::Permissions permissions, MemoryContext::MemoryAllocationHints hints)
{
    void *foundBaseAddr = findEmptyMemoryExtent(*baseAddress, length, hints);
    if (UNLIKELY(!foundBaseAddr)) {
        return -ENOMEM;
    }

    MemoryDescriptor *desc = new MemoryDescriptor;
    if (UNLIKELY(!desc)) {
        printk("Not enough kernel memory to allocate MemoryDescriptor\n");
        return -ENOMEM;
    }

    *baseAddress = foundBaseAddr;

    desc->baseAddress = *baseAddress;
    desc->length = length;
    desc->permissions = permissions;
    desc->flags = MemoryDescriptor::AnonymousMemory;
    insertMemoryDescriptor(desc);

    return 0;
}

int MemoryContext::allocateAnonymousMemory(void *baseAddress, unsigned long length, MemoryDescriptor::Permissions permissions, MemoryContext::MemoryAllocationHints hints)
{
    void *foundBaseAddr = baseAddress;
    if (UNLIKELY(!foundBaseAddr)) {
        return -ENOMEM;
    }

    MemoryDescriptor *desc = new MemoryDescriptor;
    if (UNLIKELY(!desc)) {
        printk("Not enough kernel memory to allocate MemoryDescriptor\n");
        return -ENOMEM;
    }
    desc->baseAddress = baseAddress;
    desc->length = length;
    desc->permissions = permissions;
    desc->flags = MemoryDescriptor::AnonymousMemory;
    insertMemoryDescriptor(desc);

    return 0;
}

unsigned long MemoryContext::resizeExtent(void *address, long increment)
{
    MemoryDescriptor *desc = findMemoryDescriptor(address);
    if (desc) {
        //TODO: we really have to check if we have enough space
        desc->length += increment;
        return increment;
    }

    return 0;
}

void MemoryContext::mapFileSegmentToMemory(VNode *node, void *virtualAddress, unsigned long length, unsigned long fileOffset, MemoryDescriptor::Permissions permissions)
{
    MemoryMappedFileDescriptor *mappedFileDesc = new MemoryMappedFileDescriptor();
    mappedFileDesc->baseAddress = virtualAddress;
    mappedFileDesc->length = length;
    mappedFileDesc->permissions = permissions;
    mappedFileDesc->flags = MemoryDescriptor::MemoryMappedFile;
    mappedFileDesc->node = FileSystem::VNodeManager::ReferenceVnode(node);
    insertMemoryDescriptor(mappedFileDesc);
}

int MemoryContext::releaseDescriptor(MemoryDescriptor *descriptor)
{
    for (int i = 0; i < m_descriptors->count(); i++) {
        if (m_descriptors->at(i) == descriptor) {
            m_descriptors->remove(i, 1);
            break;
        }
    }

    PagingManager::removePages(descriptor->baseAddress, roundToPageMultiples(descriptor->length));
    delete descriptor;

    return 0;
}
