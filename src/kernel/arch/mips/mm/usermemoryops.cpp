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
 *   Name: usermemoryops.h                                                 *
 *   Date: 28/12/2015                                                      *
 ***************************************************************************/

#include <mm/usermemoryops.h>

#include <arch/mips/mm/pagingmanager.h>
#include <arch/mips/mm/tlbregisters.h>
#include <arch.h>
#include <errors.h>
#include <cstdlib.h>
#include <gccbuiltins.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

extern "C"
{

bool canWriteUserMemory(void *ptr, unsigned long size)
{
    if (LIKELY(((unsigned long) ptr >= USERSPACE_LOW_ADDR) && (((unsigned long) ptr) + size <= USERSPACE_HI_ADDR))) {
       //TODO: need to check page table here
       PagingManager::lockPage(ptr);
       return true;
    } else {
       return false;
    }
}

bool canReadUserMemory(const void *ptr, unsigned long size)
{
    if (LIKELY(((unsigned long) ptr >= USERSPACE_LOW_ADDR) && (((unsigned long) ptr + size) <= USERSPACE_HI_ADDR))) {
       //TODO: need to check page table here
       PagingManager::lockPage(ptr);
       return true;
    } else {
       return false;
    }
}

int strnlenUser(userptr const char *s, int maxsize)
{
    if (LIKELY(canReadUserMemory(s, maxsize))) {
        return strlen(s);
    } else {
        DEBUG_MSG("strlenUser: Warning: cannot read 0x%p, size: %i\n", s, maxsize);
        return -EFAULT;
    }
}

int strndupUser(userptr const char *s, int maxsize, char **newString)
{
    if (LIKELY(canReadUserMemory(s, maxsize))) {
        *newString = strndup(s, maxsize);
        return 0;
    } else {
        DEBUG_MSG("strlenUser: Warning: cannot write to 0x%p, size: %i\n", s, maxsize);
        return -EFAULT;
    }
}

int strncpyFromUser(char *dest, userptr const char *src, int size)
{
    if (LIKELY(canReadUserMemory(src, size))) {
        strncpy(dest, src, size);
        return 0;
    } else {
        DEBUG_MSG("strncpyFromUser: Warning: cannot write to 0x%p, size: %i\n", src, size);
        return -EFAULT;
    }

}

int memcpyToUser(userptr void *dest, const void *src, unsigned long size)
{
    if (LIKELY(canWriteUserMemory(dest, size))) {
        memcpy(dest, src, size);
        return 0;
    } else {
        DEBUG_MSG("memcpyToUser: Warning: cannot write to 0x%p, size: %lu\n", dest, size);
        return -EFAULT;
    }
}

int memcpyFromUser(void *dest, userptr const void *src, unsigned long size)
{
    if (LIKELY(canReadUserMemory(src, size))) {
        memcpy(dest, src, size);
        return 0;
    } else {
        DEBUG_MSG("memcpyFromUser: Warning: cannot read 0x%p, size: %lu\n", src, size);
        return -EFAULT;
    }
}

int getFromUser8(uint8_t *value, uint8_t *ptr)
{
    if (LIKELY(canReadUserMemory(ptr, 1))) {
        *value = *ptr;
        return 0;
    } else {
        DEBUG_MSG("getFromUser8: Warning: cannot write to 0x%p, size: %i\n", ptr, 1);
        return -EFAULT;
    }
}

int getFromUser16(uint16_t *value, uint16_t *ptr)
{
    if (LIKELY(canReadUserMemory(ptr, 2))) {
        *value = *ptr;
        return 0;
    } else {
        DEBUG_MSG("getFromUser16: Warning: cannot write to 0x%p, size: %i\n", ptr, 2);
        return -EFAULT;
    }
}

int getFromUser32(uint32_t *value, uint32_t *ptr)
{
    if (LIKELY(canReadUserMemory(ptr, 4))) {
        *value = *ptr;
        return 0;
    } else {
        DEBUG_MSG("getFromUser32: Warning: cannot write to 0x%p, size: %i\n", ptr, 4);
        return -EFAULT;
    }
}

int getFromUser64(uint64_t *value, uint64_t *ptr)
{
    if (LIKELY(canReadUserMemory(ptr, 8))) {
        *value = *ptr;
        return 0;
    } else {
        DEBUG_MSG("getFromUser64: Warning: cannot write to 0x%p, size: %i\n", ptr, 8);
        return -EFAULT;
    }
}

int putToUser8(uint8_t value, uint8_t *ptr)
{
    if (LIKELY(canWriteUserMemory(ptr, 1))) {
        *ptr = value;
        return 0;
    } else {
        DEBUG_MSG("putToUser8: Warning: cannot write to 0x%p, size: %i\n", ptr, 1);
        return -EFAULT;
    }
}

int putToUser16(uint16_t value, uint16_t *ptr)
{
    if (LIKELY(canWriteUserMemory(ptr, 2))) {
        *ptr = value;
        return 0;
    } else {
        DEBUG_MSG("putToUser16: Warning: cannot write to 0x%p, size: %i\n", ptr, 2);
        return -EFAULT;
    }
}

int putToUser32(uint32_t value, uint32_t *ptr)
{
    if (LIKELY(canWriteUserMemory(ptr, 4))) {
        *ptr = value;
        return 0;
    } else {
        DEBUG_MSG("putToUser32: Warning: cannot write to 0x%p, size: %i\n", ptr, 4);
        return -EFAULT;
    }
}

int putToUser64(uint64_t value, uint64_t *ptr)
{
    if (LIKELY(canWriteUserMemory(ptr, 8))) {
        *ptr = value;
        return 0;
    } else {
        DEBUG_MSG("putToUser64: Warning: cannot write to 0x%p, size: %i\n", ptr, 8);
        return -EFAULT;
    }
}

}
