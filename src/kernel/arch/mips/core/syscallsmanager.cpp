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
 *   Name: syscallsmanager.cpp                                             *
 *   Date: 19/10/2015                                                      *
 ***************************************************************************/

#include <core/syscallsmanager.h>

#include <uapi/syscallsnr.h>
#include <core/printk.h>
#include <core/systemtimer.h>
#include <filesystem/vfs.h>
#include <task/task.h>
#include <arch/mips/core/cpuregistersframe.h>
#include <task/archthreadsmanager.h>
#include <task/userprocessimage.h>
#include <task/userprocsmanager.h>

#define SYSCALL_FUNCTION_PTR unsigned long (*)(unsigned long, unsigned long, unsigned long)
#define SYSCALLTABLE_SIZE 256

unsigned long (*syscallsTable[SYSCALLTABLE_SIZE])(unsigned long, unsigned long, unsigned long);

void SyscallsManager::init()
{
}

void SyscallsManager::registerSyscall(int num, void *funcPtr)
{
    if ((num >= __NR_SYSCALLS) && (num < __NR_SYSCALLS + SYSCALLTABLE_SIZE)) {
        syscallsTable[num - __NR_SYSCALLS] = (SYSCALL_FUNCTION_PTR) funcPtr;

    } else {
        printk("Failed to register %p (out of range syscall number %i)\n", funcPtr, num);
    }
}

void SyscallsManager::unregisterSyscall(int num)
{
    syscallsTable[num] = NULL;
}

extern "C" unsigned long syscallISR(unsigned long nr, unsigned long a0, unsigned long a1, unsigned long a2)
{
    if (nr >= __NR_SYSCALLS) {
        unsigned long syscallNumber = nr - __NR_SYSCALLS;

        if (syscallNumber < SYSCALLTABLE_SIZE && syscallsTable[syscallNumber]) {
            return syscallsTable[syscallNumber](a0, a1, a2);

        } else {
            printk("Invalid syscall number: %i (a0: %x, a1: %x, a3: %x)\n", nr, a0, a1, a2);
            return -EINVAL;
        }
    } else {
        printk("Invalid syscall number: %i (a0: %x, a1: %x, a3: %x)\n", nr, a0, a1, a2);
        return -EINVAL;
    }
}
