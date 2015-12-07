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
 *   Name: memoryuapi.cpp                                                  *
 *   Date: 23/11/2015                                                      *
 ***************************************************************************/

#include <uapi/memoryuapi.h>

#include <mm/memorycontext.h>
#include <task/scheduler.h>
#ifndef ARCH_IA32
#include <uapi/syscallsnr.h>
#endif
#include <core/syscallsmanager.h>

#define PROT_READ       0x1
#define PROT_WRITE      0x2
#define PROT_EXEC       0x4

#define MAP_SHARED      0x01
#define MAP_PRIVATE     0x02
#define MAP_TYPE        0x0F

#define MAP_FIXED       0x10
#define MAP_ANONYMOUS   0x20

#ifdef ARCH_IA32_NATIVE

struct MmapArgs
{
	unsigned long addr;
	unsigned long len;
	unsigned long prot;
	unsigned long flags;
	long fd;
	unsigned long offset;
};

uint32_t mmap_i386(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    MmapArgs *args = (MmapArgs *) ebx;
    return (uint32_t) MemoryUAPI::mmap((void *) args->addr, (size_t) args->len, (int) args->prot, (int) args->flags, (int) args->fd, (size_t) args->offset);
}

#endif

inline MemoryDescriptor::Permissions protFlagsToPermissions(int prot)
{
    MemoryDescriptor::Permissions permissions = MemoryDescriptor::NoAccess;
    if (prot & PROT_READ) {    
        permissions = (MemoryDescriptor::Permissions) (permissions | MemoryDescriptor::ReadPermission);
    }
    if (prot & PROT_WRITE) {    
        permissions = (MemoryDescriptor::Permissions) (permissions | MemoryDescriptor::WritePermission);
    }
    if (prot & PROT_EXEC) {    
        permissions = (MemoryDescriptor::Permissions) (permissions | MemoryDescriptor::WritePermission);
    }
    return permissions;
}

void MemoryUAPI::init()
{
#ifndef ARCH_IA32
    SyscallsManager::registerSyscall(__NR_BRK, (void *)  brk);
#endif
}

void *MemoryUAPI::brk(void *ptr)
{
    ProcessControlBlock *process = Scheduler::currentThread()->parentProcess;

    if (ptr != NULL){
        MemoryDescriptor *dataSegmentDescriptor = process->memoryContext->findMemoryDescriptor(process->dataSegmentStart);
        if (UNLIKELY(!dataSegmentDescriptor)) {
            printk("Error: data segment does not point to any valid MemoryDescriptor\n");
            return NULL;
        }

        if (ptr > process->dataSegmentEnd) {
            // check if we are going to overlap any existing mapping
            if (UNLIKELY(process->memoryContext->countDescriptorsByRange(dataSegmentDescriptor, (void *) ((unsigned long) ptr - (unsigned long) process->dataSegmentEnd)))) {
                return (void *) -ENOMEM;
            }
        } else {
            // it doesn't make any sense to shrink the data segment to a negative size
            // TODO: specification doesn't say anything about this, and I'm not sure if ENOMEM is the
            // error that has to be returned
            if (UNLIKELY(ptr < process->dataSegmentStart)) {
                return (void *) -ENOMEM;
            }
        }

        process->dataSegmentEnd = (void *) ((unsigned long) process->dataSegmentStart +
                                            (unsigned long) process->memoryContext->resizeExtent(dataSegmentDescriptor, (long) ptr - (long) process->dataSegmentEnd));
    }

    return process->dataSegmentEnd;
}

unsigned long MemoryUAPI::mmap(void *addr, unsigned long length, unsigned long prot, unsigned long flags, long fd, unsigned long offset)
{
    // flags must be either MAP_SHARED or MAP_PRIVATE.
    if (((flags & (MAP_PRIVATE | MAP_SHARED)) == (MAP_PRIVATE | MAP_SHARED)) || ((flags & (MAP_PRIVATE | MAP_SHARED)) == 0)) {
        return -EINVAL;
    }

    ProcessControlBlock *process = Scheduler::currentThread()->parentProcess;

    MemoryContext::MemoryAllocationHints hints = MemoryContext::NoHints;
    if (flags & MAP_FIXED) {
        hints = (MemoryContext::MemoryAllocationHints) MemoryContext::FixedHint;
    }

    MemoryDescriptor::Permissions permissions = protFlagsToPermissions(prot);

    if (flags & MAP_ANONYMOUS) {
        void *newAddr = addr;
        process->memoryContext->allocateAnonymousMemory(&newAddr, length, permissions, hints);
        return (unsigned long) newAddr;
       
    } else {
        if (!((fd >= 0) && (fd < process->openFiles->size()))) {
            return (unsigned long) -EBADF;
        }
        FileDescriptor *fdesc = process->openFiles->at(fd);
        if (fdesc == NULL) {
            return (unsigned long) -EBADF;
        }
        void *ret = FS_CALL(fdesc->node, mmap)(fdesc->node, addr, length, prot, flags, fd, offset);
        if (ret != 0) {
            return (unsigned long) ret;
        }
        process->memoryContext->mapFileSegmentToMemory(fdesc->node, addr, length, offset, permissions);
        return (unsigned long) addr;
    }
}

unsigned long MemoryUAPI::munmap(void *addr, unsigned long length)
{

    ProcessControlBlock *process = Scheduler::currentThread()->parentProcess;

    QList<MemoryDescriptor *> *descriptors = process->memoryContext->findMemoryDescriptorsByRange(addr, (char *) addr + length);
    for (int i = 0; i < descriptors->count(); i++) {
        MemoryDescriptor *desc = descriptors->at(i);
        process->memoryContext->releaseDescriptor(desc);
    }
    return 0;
}

//TODO: stub
int MemoryUAPI::mlock(const void *addr, unsigned long len)
{
    return -ENOMEM;
}

//TODO: stub
int MemoryUAPI::munlock(const void *addr, unsigned long len)
{
    return -ENOMEM;
}

//TODO: stub
int MemoryUAPI::mlockall(int flags)
{
    return -ENOMEM;
}

//TODO: stub
int MemoryUAPI::munlockall()
{
    return -ENOMEM;
}

int MemoryUAPI::mprotect(void *addr, unsigned long len, int prot)
{
    if (UNLIKELY(addr == NULL)) {
        return -EINVAL;
    }
    if (UNLIKELY(!((((unsigned long) addr) < USERSPACE_LOW_ADDR) && ((((unsigned long) addr) + len) > USERSPACE_HI_ADDR)))) {
        return -ENOMEM;
    }
    //TODO: We should support any page size
    if (UNLIKELY(((unsigned long) addr) % 4096)) {
        return -EINVAL;
    }

    MemoryDescriptor::Permissions permissions = protFlagsToPermissions(prot);

    ProcessControlBlock *process = Scheduler::currentThread()->parentProcess;
    QList<MemoryDescriptor *> *descriptors = process->memoryContext->findMemoryDescriptorsByRange(addr, ((char *) addr) + len);
    for (int i = 0; i < descriptors->count(); i++) {
        process->memoryContext->updatePermissions(descriptors->at(i), permissions);
    }
    delete descriptors;

    return 0;
}

//TODO: stub
int MemoryUAPI::msync(void *addr, unsigned long length, int flags)
{
    return -ENOMEM;
}

//TODO stub
unsigned long MemoryUAPI::mremap(void *old_address, unsigned long old_size, unsigned long new_size, int flags, ... /* void *new_address */)
{
    return -ENOMEM;
}
