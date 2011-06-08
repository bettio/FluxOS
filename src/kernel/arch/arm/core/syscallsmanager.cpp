/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 29/09/2010                                                      *
 ***************************************************************************/

#include <arch/arm/core/syscallsmanager.h>

#include <arch/arm/core/exceptionsvector.h>
#include <core/printk.h>
//#include <core/time.h>
#include <core/system.h>
#include <filesystem/vfs.h>
#include <filesystem/fscalls.h>
//#include <mm/vmem.h>
#include <task/task.h>

#define SYSCALL_FUNCTION_PTR uint32_t (*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
#define SYSCALL_MAXNUM 256

extern "C" void swiHandler();

uint32_t (*SyscallsTable[SYSCALL_MAXNUM])(uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r4, uint32_t r5, uint32_t r6);

void SyscallsManager::init()
{
    ExceptionsVector::setHandler(ExceptionsVector::SWI, (void *) swiHandler);

    for(int i = 0; i < SYSCALL_MAXNUM; i++){
        registerSyscall(i, nullSysCallHandler);
    }
    registerDefaultSyscalls();
}

void SyscallsManager::registerSyscall(int num, uint32_t (*func)(uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r4, uint32_t r5, uint32_t r6))
{
    SyscallsTable[num] = func;
}

void SyscallsManager::unregisterSyscall(int num)
{
    SyscallsTable[num] = nullSysCallHandler;
}

uint32_t SyscallsManager::nullSysCallHandler(uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r4, uint32_t r5, uint32_t r6)
{
    printk("Null System Call Handler:\n"
        "An invalid system call number has been used\n"
        " - r1: 0x%x, r2: 0x%x\n"
        " - r3: 0x%x, r4: 0x%x\n"
        " - r5: 0x%x, r6: 0x%x\n",
        r1, r2, r3, r4, r5, r6);

    return -EFAULT;
}

void SyscallsManager::registerDefaultSyscalls()
{
    //registerSyscall((SYSCALL_FUNCTION_PTR) exit, 1);
    //registerSyscall((SYSCALL_FUNCTION_PTR) fork, 2);
    registerSyscall(3, (SYSCALL_FUNCTION_PTR) read);
    registerSyscall(4, (SYSCALL_FUNCTION_PTR) write);
    registerSyscall(5, (SYSCALL_FUNCTION_PTR) open);
    registerSyscall(6, (SYSCALL_FUNCTION_PTR) close);
    //registerSyscall((SYSCALL_FUNCTION_PTR) waitpid, 7);
    //registerSyscall((SYSCALL_FUNCTION_PTR) execve, 11);
    registerSyscall(12, (SYSCALL_FUNCTION_PTR) chdir);
    //registerSyscall((SYSCALL_FUNCTION_PTR) time, 13);
    registerSyscall(15, (SYSCALL_FUNCTION_PTR) chmod);
    registerSyscall(16, (SYSCALL_FUNCTION_PTR) lchown);
    registerSyscall(18, (SYSCALL_FUNCTION_PTR) stat);
    registerSyscall(19, (SYSCALL_FUNCTION_PTR) lseek);
    //registerSyscall((SYSCALL_FUNCTION_PTR) getpid, 20);
    //registerSyscall((SYSCALL_FUNCTION_PTR) mount, 21);
    //registerSyscall((SYSCALL_FUNCTION_PTR) umount, 22);
    //registerSyscall((SYSCALL_FUNCTION_PTR) setuid, 23);
    //registerSyscall((SYSCALL_FUNCTION_PTR) getuid, 24);
    registerSyscall(28, (SYSCALL_FUNCTION_PTR) fstat);
    //registerSyscall((SYSCALL_FUNCTION_PTR) kill, 37);
    //registerSyscall((SYSCALL_FUNCTION_PTR) brk, 45);
    //registerSyscall((SYSCALL_FUNCTION_PTR) setgid, 46);
    //registerSyscall((SYSCALL_FUNCTION_PTR) getgid, 47);
    registerSyscall(54, (SYSCALL_FUNCTION_PTR) ioctl);
    registerSyscall(55, (SYSCALL_FUNCTION_PTR) fcntl);
    //registerSyscall((SYSCALL_FUNCTION_PTR) getppid, 64);
    //registerSyscall((SYSCALL_FUNCTION_PTR) sethostname, 74);
    //registerSyscall((SYSCALL_FUNCTION_PTR) mmap, 90);
    registerSyscall(92, (SYSCALL_FUNCTION_PTR) truncate);
    registerSyscall(93, (SYSCALL_FUNCTION_PTR) ftruncate);
    registerSyscall(94, (SYSCALL_FUNCTION_PTR) fchmod);
    registerSyscall(95, (SYSCALL_FUNCTION_PTR) fchown);
    registerSyscall(107, (SYSCALL_FUNCTION_PTR) lstat);
    registerSyscall(85, (SYSCALL_FUNCTION_PTR) readlink);
    registerSyscall(118, (SYSCALL_FUNCTION_PTR) fsync);
    //registerSyscall((SYSCALL_FUNCTION_PTR) setdomainname, 121);
    registerSyscall(122, (SYSCALL_FUNCTION_PTR) Uname);
    registerSyscall(141, (SYSCALL_FUNCTION_PTR) getdents);
    registerSyscall(148, (SYSCALL_FUNCTION_PTR) fdatasync);
    registerSyscall(180, (SYSCALL_FUNCTION_PTR) pread);
    registerSyscall(181, (SYSCALL_FUNCTION_PTR) pwrite);
    registerSyscall(182, (SYSCALL_FUNCTION_PTR) chown);
    registerSyscall(183, (SYSCALL_FUNCTION_PTR) getcwd);
    //registerSyscall((SYSCALL_FUNCTION_PTR) CreateProcess, 220);
}
