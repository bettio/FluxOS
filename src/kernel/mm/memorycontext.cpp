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

    printk("[Process %i] Segmentation Fault: instruction at 0x%p tried to %s at %s memory address 0x%p.\n", pid, faultPC, memoryOperationToString(op), memoryDescriptorTypeString, faultAddress);
    while(1);
}

MemoryContext::MemoryContext()
{
    m_descriptors = new QList<MemoryDescriptor *>();
}

void MemoryContext::handlePageFault(void *faultAddress, void *faultPC, UserspaceMemoryManager::MemoryOperation op, UserspaceMemoryManager::PageFaultFlags flags)
{
    MemoryDescriptor *mDesc = NULL;
    for (int i = 0; i < m_descriptors->count(); i++) {
        MemoryDescriptor *d = m_descriptors->at(i);
        if (((unsigned long) faultAddress >= (unsigned long) d->baseAddress) &&
            ((unsigned long) faultAddress < ((unsigned long) d->baseAddress) + d->length)) {
            mDesc = d;
            break;
        }
    }

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
        printk("missing page fault\n");
        #ifdef ARCH_IA32_NATIVE
            PagingManager::newPage((uint32_t) faultAddress);
        #endif
    }
}

int MemoryContext::insertMemoryDescriptor(MemoryDescriptor *descriptor)
{
    m_descriptors->append(descriptor);
    return 0;
}

void *MemoryContext::findMemoryExtent(void *baseAddress, unsigned long length, MemoryContext::MemoryAllocationHints hints)
{
    return baseAddress;
}

int MemoryContext::allocateAnonymousMemory(void *baseAddress, unsigned long length, MemoryDescriptor::Permissions permissions, MemoryContext::MemoryAllocationHints hints)
{
    void *foundBaseAddr = findMemoryExtent(baseAddress, length, hints);
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
