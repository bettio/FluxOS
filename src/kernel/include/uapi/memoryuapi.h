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
 *   Name: processuapi.h                                                   *
 *   Date: 17/11/2015                                                      *
 ***************************************************************************/

#ifndef _UAPI_MEMORYUAPI_H_
#define _UAPI_MEMORYUAPI_H_

#include <kdef.h>

uint32_t mmap_i386(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t);

class MemoryUAPI
{
    public:
        static void init();
        static void *brk(void *ptr);
        static unsigned long mmap(void *addr, unsigned long length, unsigned long prot, unsigned long flags, long fd, unsigned long offset);
        static unsigned long munmap(void *addr, unsigned long length);        
        static int mprotect(void *addr, unsigned long len, int prot);
        static int msync(void *addr, unsigned long length, int flags);
        static unsigned long mremap(void *old_address, unsigned long old_size, unsigned long new_size, int flags, ... /* void *new_address */);
        static int mlock(const void *addr, unsigned long len);
        static int munlock(const void *addr, unsigned long len);
        static int mlockall(int flags);
        static int munlockall();
};

#endif
