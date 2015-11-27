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

#define PROT_READ       0x1
#define PROT_WRITE      0x2

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
	unsigned long fd;
	unsigned long offset;
};

uint32_t mmap_i386(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    MmapArgs *args = (MmapArgs *) ebx;
    return (uint32_t) MemoryUAPI::mmap((void *) args->addr, (size_t) args->len, (int) args->prot, (int) args->flags, (int) args->fd, (size_t) args->offset);
}

#endif

void MemoryUAPI::init()
{

}

void *MemoryUAPI::brk(void *ptr)
{
    ProcessControlBlock *process = Scheduler::currentThread()->parentProcess;

    if (ptr != NULL){
        if (ptr > process->dataSegmentEnd) {
            process->dataSegmentEnd = (void *) ((unsigned long) process->dataSegmentStart +
                                                (unsigned long) process->memoryContext->resizeExtent(process->dataSegmentStart, (long) ptr - (long) process->dataSegmentEnd));
        } else {
            printk("Error cannot shrink data segment\n");
        }
    }

    return process->dataSegmentEnd;
}

unsigned long MemoryUAPI::mmap(void *addr, unsigned long length, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long offset)
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

    MemoryDescriptor::Permissions permissions = MemoryDescriptor::NoAccess;
    if (prot & PROT_READ) {    
        permissions = (MemoryDescriptor::Permissions) (permissions | MemoryDescriptor::ReadPermission);
    }
    if (prot & PROT_WRITE) {    
        permissions = (MemoryDescriptor::Permissions) (permissions | MemoryDescriptor::WritePermission);
    }

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

int MemoryUAPI::mlock(const void *addr, unsigned long len)
{

}

int MemoryUAPI::munlock(const void *addr, unsigned long len)
{

}

int MemoryUAPI::mlockall(int flags)
{

}

int MemoryUAPI::munlockall()
{

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

    ProcessControlBlock *process = Scheduler::currentThread()->parentProcess;
    QList<MemoryDescriptor *> *descriptors = process->memoryContext->findMemoryDescriptorsByRange(addr, ((char *) addr) + len);
}

int MemoryUAPI::msync(void *addr, unsigned long length, int flags)
{

}

unsigned long MemoryUAPI::mremap(void *old_address, unsigned long old_size, unsigned long new_size, int flags, ... /* void *new_address */)
{

}
