/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: malloc.cpp                                                      *
 *   Date: 05/02/2006                                                      *
 ***************************************************************************/

#include <cstdlib.h>

#include <arch.h>
#include <core/printk.h>

#ifdef ARCH_UMF
    #include <arch/umf/core/hostsyscalls.h>
#endif

#ifndef KERNEL_HEAP_START
#define KERNEL_HEAP_START 0
#endif

unsigned long mem = KERNEL_HEAP_START;
    
extern "C"
{
    void initmem()
    {
        #ifdef ARCH_UMF_BRK
            mem = HostSysCalls::brk((void *) 0);
        #endif

        mem += (8 - (mem % 8));

        #ifdef ARCH_UMF_BRK
            HostSysCalls::brk((void *) mem);
        #endif
    }

    void *calloc(size_t num, size_t size)
    {
        unsigned long tmp;

        tmp = mem + (8 - ((mem + size) % 8));

        mem += num*size + (8 - ((mem + size) % 8));
        #ifdef ARCH_UMF_BRK
            HostSysCalls::brk((void *) mem);
        #endif

        return (void*) tmp;
    }

    void free(void *ptr)
    {
        //printk("free: %x\n", ptr);
        #ifdef ARCH_UMF
        #ifndef ARCH_UMF_BRK
            HostSysCalls::munmap(((unsigned long *) ptr) - 1, *(((unsigned long *) ptr) - 1) + sizeof(unsigned long));
        #endif
        #endif
    }

    void *malloc(size_t size)
    {
        unsigned long tmp;

        tmp = mem;

        mem += (1024*1024);//size + (8 - ((mem + size) % 8));
        #ifdef ARCH_UMF_BRK
        
            HostSysCalls::brk((void *) mem);
        #endif

        #ifdef ARCH_UMF
        #ifndef ARCH_UMF_BRK
            tmp = (unsigned long) HostSysCalls::mmap(0, size + sizeof(unsigned long), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
            *((unsigned long *) tmp) = size;
            tmp += sizeof(unsigned long);
        #endif
        #endif
    
        return (void*) tmp; 
    }

    void *realloc(void *ptr, size_t size)
    {
        void *newPtr = malloc(size);
        memcpy(newPtr, ptr, size);

        return (void *) newPtr;
    }
}
