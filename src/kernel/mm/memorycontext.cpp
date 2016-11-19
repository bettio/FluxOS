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
#include <task/task.h>
#include <uapi/processuapi.h>

#ifdef ARCH_IA32_NATIVE
#include <arch/ia32/mm/pagingmanager.h>
#endif
#ifdef ARCH_MIPS
 #include <arch/mips/mm/pagingmanager.h>
#endif

#define ENABLE_DEBUG_MSG 0
#include <debugmacros.h>

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
    ProcessUAPI::kill(pid, SIGSEGV); //TODO: do not use UAPI to send signals to processes from kernel space
    schedule();
}

MemoryContext::MemoryContext()
{
    m_descriptors = new QList<MemoryDescriptor *>();

    pageSize = 4096;
    pageSizeShift = 12; // 4096 bytes
    userVirtualMemLowAddress = USERSPACE_LOW_ADDR;
    userVirtualMemSize = USERSPACE_HI_ADDR - USERSPACE_LOWER_ADDR;

    m_vmemAlloc.init(userVirtualMemSize >> 12);
}

MemoryContext::~MemoryContext()
{
    while (m_descriptors->count()) {
        releaseDescriptor(m_descriptors->at(m_descriptors->count() - 1));
    }
    delete m_descriptors;
}

void MemoryContext::printAllDescriptors()
{
    for (int i = 0; i < m_descriptors->count(); i++) {
        printk("baseAddress: 0x%x, length: %i\n", m_descriptors->at(i)->baseAddress, m_descriptors->at(i)->length);
    }
}

inline unsigned long roundToPageMultiples(unsigned long l)
{
    return ((l & 0xFFFFF000) + ((l & 0xFFF) ? 0x1000 : 0));
}

void *MemoryContext::allocVirtualMemory(unsigned long size)
{
    DEBUG_MSG("MemoryContext::allocVirtualMemory(%li) ", size);
    unsigned long pageIndex = m_vmemAlloc.allocateBlocks(size >> pageSizeShift);
    void *rptr = (void *) ((pageIndex << pageSizeShift) + userVirtualMemLowAddress);
    DEBUG_MSG("-> %x\n", rptr);
    return rptr;
}

void MemoryContext::allocVirtualMemory(void *address, unsigned long size)
{
    DEBUG_MSG("MemoryContext::allocVirtualMemory(%p, %li)\n", address, size);
    unsigned long relativeAddress = ((unsigned long) address) - userVirtualMemLowAddress;
    m_vmemAlloc.allocateBlocks(relativeAddress >> pageSizeShift, size >> pageSizeShift);
}

void MemoryContext::freeVirtualMemory(void *address, unsigned long size)
{
    DEBUG_MSG("MemoryContext::freeVirtualMemory(%p, %li)\n", address, size);
    unsigned long relativeAddress = ((unsigned long) address) - userVirtualMemLowAddress;
    m_vmemAlloc.freeBlocks(relativeAddress, size >> pageSizeShift);
}

MemoryDescriptor *MemoryContext::findMemoryDescriptor(void *address) const
{
    DEBUG_MSG("MemoryContext::findMemoryDescriptor(0x%p)\n", address);
    for (int i = 0; i < m_descriptors->count(); i++) {
        MemoryDescriptor *d = m_descriptors->at(i);
        DEBUG_MSG("checking: 0x%x - 0x%x\n", d->baseAddress, d->baseAddress + roundToPageMultiples(d->length));
        if (((unsigned long) address >= (unsigned long) d->baseAddress) &&
            ((unsigned long) address < ((unsigned long) d->baseAddress) + roundToPageMultiples(d->length))) {
            DEBUG_MSG("found\n");
            return d;
        }
    }
    DEBUG_MSG("not found\n");

    return NULL;
}

QList<MemoryDescriptor *> *MemoryContext::findMemoryDescriptorsByRange(void *low, void *hi) const
{
//    printk("range: %p - %p\n", low, hi);
    QList<MemoryDescriptor *> *foundDescriptors = new QList<MemoryDescriptor *>();

    for (int i = 0; i < m_descriptors->count(); i++) {
        MemoryDescriptor *d = m_descriptors->at(i);
//        printk("checking: 0x%x - 0x%x\n", d->baseAddress, ((unsigned long) d->baseAddress) + roundToPageMultiples(d->length));
        if (
            ( ((unsigned long) low) < (((unsigned long) d->baseAddress) + roundToPageMultiples(d->length)) ) &&
            ( ((unsigned long) hi) >= (((unsigned long) d->baseAddress) + roundToPageMultiples(d->length)) )
           ) {
//        printk("found: 0x%x - 0x%x\n", d->baseAddress, ((unsigned long) d->baseAddress) + roundToPageMultiples(d->length));
            foundDescriptors->append(d);
        }
    }

    return foundDescriptors;

}

int MemoryContext::countDescriptorsByRange(void *low, void *hi) const
{
    DEBUG_MSG("MemoryContext::countDescriptorsByRange(0x%p, 0x%p)\n", low, hi);
    int count = 0;

    for (int i = 0; i < m_descriptors->count(); i++) {
        MemoryDescriptor *d = m_descriptors->at(i);
        DEBUG_MSG("count checking: 0x%x - 0x%x\n", d->baseAddress, ((unsigned long) d->baseAddress) + roundToPageMultiples(d->length));
        if (
            ( ((unsigned long) low) <= (((unsigned long) d->baseAddress) + roundToPageMultiples(d->length)) ) &&
            ( ((unsigned long) hi) >= (((unsigned long) d->baseAddress) + roundToPageMultiples(d->length)) )
           ) {
            DEBUG_MSG("count found: 0x%x - 0x%x\n", d->baseAddress, ((unsigned long) d->baseAddress) + roundToPageMultiples(d->length));
            count++;
        }
    }

    return count;
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
       unsigned long npFlags = (mDesc->permissions & MemoryDescriptor::WritePermission) ? PagingManager::Write : 0;
       PagingManager::newPage((uint32_t) faultAddress, npFlags);

    } else if (flags & UserspaceMemoryManager::MissingPageFault && (mDesc->flags == MemoryDescriptor::MemoryMappedFile)) {
       MemoryMappedFileDescriptor *mfDesc = (MemoryMappedFileDescriptor *) mDesc;

       unsigned long npFlags = (mfDesc->permissions & MemoryDescriptor::WritePermission) ? PagingManager::Write : 0;
       PagingManager::newPage((uint32_t) faultAddress, npFlags);

       unsigned long virtualPageAddress = (((unsigned long) faultAddress) & 0xFFFFF000);
       unsigned long offset = virtualPageAddress - ((unsigned long) mfDesc->baseAddress);
       int res = FS_CALL(mfDesc->node, read)(mfDesc->node, offset, (char *) virtualPageAddress, 4096);
       if (res < 0) {
           printk("I/O error while loading page (read bytes: %i)\n", res);
           segmentationFault(faultAddress, faultPC, op, flags, "missing page");
       }
   } else if (flags & UserspaceMemoryManager::MissingPageFault && (mDesc->flags == MemoryDescriptor::DelayedLoadFile)) {
       DelayedLoadFileMemory *mfDesc = (DelayedLoadFileMemory *) mDesc;

       unsigned long npFlags = (mfDesc->permissions & MemoryDescriptor::WritePermission) ? PagingManager::Write : 0;
       PagingManager::newPage((uint32_t) faultAddress, npFlags);

       unsigned long virtualPageAddress = (((unsigned long) faultAddress) & 0xFFFFF000);
       int res = FS_CALL(mfDesc->node, read)(mfDesc->node, mfDesc->fileOffset, ((char *) virtualPageAddress) + mfDesc->pageOffset, mfDesc->fileLen);
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

    if (baseAddress) {
        // make sure we are not trying to allocate overlapping memory regions
        QList<MemoryDescriptor *> *descriptors = findMemoryDescriptorsByRange(baseAddress, (char *) baseAddress + length);
        if (!descriptors->isEmpty() && (hints & FixedHint)) {
            printk("WARNING: Tried to allocate already used memory region\n");
            delete descriptors;
            return NULL;
        } else if (descriptors->isEmpty()) {
            allocVirtualMemory(baseAddress, roundToPageMultiples(length));
        }
        delete descriptors;
    }
    if (baseAddress == NULL) {
        newAddress = allocVirtualMemory(roundToPageMultiples(length));
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
    void *foundBaseAddr = findEmptyMemoryExtent(baseAddress, length, hints);
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


int MemoryContext::allocateDelayedLoadedFile(void *baseAddress, unsigned long length,
                              VNode *node, unsigned long fOffset, int pageOffset, int fLen,
                              MemoryDescriptor::Permissions permissions, MemoryContext::MemoryAllocationHints hints)
{
    void *foundBaseAddr = findEmptyMemoryExtent(baseAddress, length, hints);
    if (UNLIKELY(!foundBaseAddr)) {
        return -ENOMEM;
    }

    DelayedLoadFileMemory *desc = new DelayedLoadFileMemory;
    if (UNLIKELY(!desc)) {
        printk("Not enough kernel memory to allocate MemoryDescriptor\n");
        return -ENOMEM;
    }
    desc->baseAddress = baseAddress;
    desc->length = length;
    desc->permissions = permissions;
    desc->flags = MemoryDescriptor::DelayedLoadFile;

    desc->node = node;
    desc->fileOffset = fOffset;
    desc->pageOffset = pageOffset;
    desc->fileLen = fLen;

    insertMemoryDescriptor(desc);

    return 0;
}


long MemoryContext::resizeExtent(MemoryDescriptor *desc, long increment)
{
    DEBUG_MSG("MemoryContext::resizeExtent(desc->baseAddress: 0x%p, %i)\n", desc->baseAddress, increment);
    void *endOfDescriptor = (void *) roundToPageMultiples(((unsigned long) desc->baseAddress) + desc->length);
    if (increment > 0) {
        int delta = roundToPageMultiples(increment);
        allocVirtualMemory(endOfDescriptor, delta);

    } else if (increment < 0) {
        int delta = (-increment) & ~(pageSize - 1);
        freeVirtualMemory((void *) ((unsigned long) endOfDescriptor - delta), delta);
        PagingManager::removePages((void *) ((unsigned long) endOfDescriptor - delta), delta);
    }

    desc->length += increment;
    return increment;
}

//TODO: map file to a certain fixed memory address
void *MemoryContext::mapFileSegmentToMemory(VNode *node, void *virtualAddress, unsigned long length, unsigned long fileOffset, MemoryDescriptor::Permissions permissions)
{
    void *foundBaseAddr = findEmptyMemoryExtent(virtualAddress, length, FixedHint);
    if (UNLIKELY(!foundBaseAddr)) {
        printk("Error: mapFileSegment: ENOMEM\n");
        return (void *) -ENOMEM;
    }

    MemoryMappedFileDescriptor *mappedFileDesc = new MemoryMappedFileDescriptor();
    if (IS_NULL_PTR(mappedFileDesc)) {
        return (void *) -ENOMEM;
    }
    mappedFileDesc->baseAddress = foundBaseAddr;
    mappedFileDesc->length = length;
    mappedFileDesc->permissions = permissions;
    mappedFileDesc->flags = MemoryDescriptor::MemoryMappedFile;
    mappedFileDesc->node = FileSystem::VNodeManager::ReferenceVnode(node);

    insertMemoryDescriptor(mappedFileDesc);

    return foundBaseAddr;
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

    if (descriptor->flags == MemoryDescriptor::MemoryMappedFile) {
        MemoryMappedFileDescriptor *mappedFileDescriptor = (MemoryMappedFileDescriptor *) descriptor;
        FileSystem::VNodeManager::PutVnode(mappedFileDescriptor->node);
    }

    delete descriptor;

    return 0;
}

int MemoryContext::updatePermissions(MemoryDescriptor *descriptor, MemoryDescriptor::Permissions permissions)
{
   descriptor->permissions = permissions;
   // TODO: update protection at page level and pay attention to copy on write
}

MemoryDescriptor *MemoryContext::makeCOWAnonymousMemory(MemoryDescriptor *descriptor)
{
    int pagesNum = (descriptor->length >> 12) + ((descriptor->length & 0xFFF) != 0);

    CopyOnWriteAnonMemory *cowMemory = new CopyOnWriteAnonMemory;
    if (UNLIKELY(!cowMemory)) {
        return NULL;
    }
    cowMemory->pageRefCounter = new uint8_t[pagesNum];
    if (UNLIKELY(!cowMemory->pageRefCounter)) {
        delete cowMemory;
        return NULL;
    }
    for (int i = 0; i < pagesNum; i++) {
        cowMemory->pageRefCounter[i] = 2;
    }

    MemoryDescriptor *newDescriptor = new MemoryDescriptor;
    if (UNLIKELY(!newDescriptor)) {
        delete cowMemory->pageRefCounter;
        delete cowMemory;
        return NULL;
    }

    descriptor->resource = cowMemory;

    newDescriptor->baseAddress = NULL;
    newDescriptor->length = descriptor->length;
    newDescriptor->permissions = descriptor->permissions;
    newDescriptor->flags = MemoryDescriptor::AnonymousMemory;
    newDescriptor->resource = cowMemory;

    return newDescriptor;
}
