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

#include <arch/mips/core/syscallsmanager.h>

#include <arch/mips/uapi/syscallsnr.h>
#include <core/printk.h>
#include <core/system.h>
#include <filesystem/vfs.h>
#include <filesystem/fscalls.h>
#include <task/task.h>
#include <arch/mips/core/cpuregistersframe.h>
#include <task/archthreadsmanager.h>

#define SYSCALL_FUNCTION_PTR unsigned long (*)(unsigned long, unsigned long, unsigned long)
#define SYSCALLTABLE_SIZE 256

int execve(const char *filename, char *const argv[], char *const envp[]);

unsigned long (*syscallsTable[SYSCALLTABLE_SIZE])(unsigned long, unsigned long, unsigned long);

void SyscallsManager::init()
{
    registerDefaultSyscalls();
}

void SyscallsManager::registerSyscall(int num, unsigned long (*func)(unsigned long, unsigned long, unsigned long))
{
    if ((num >= __NR_SYSCALLS) && (num < __NR_SYSCALLS + SYSCALLTABLE_SIZE)) {
        syscallsTable[num - __NR_SYSCALLS] = func;

    } else {
        printk("Failed to register out of range syscall number: %i\n", num);
    }
}

void SyscallsManager::unregisterSyscall(int num)
{
    syscallsTable[num] = NULL;
}

void SyscallsManager::registerDefaultSyscalls()
{
//    registerSyscall(__NR_EXIT, (SYSCALL_FUNCTION_PTR) exit);
//    registerSyscall(__NR_FORK, (SYSCALL_FUNCTION_PTR) fork);
    registerSyscall(__NR_READ, (SYSCALL_FUNCTION_PTR) read);
    registerSyscall(__NR_WRITE, (SYSCALL_FUNCTION_PTR) write);
    registerSyscall(__NR_OPEN, (SYSCALL_FUNCTION_PTR) open);
//    registerSyscall(__NR_CLOSE, (SYSCALL_FUNCTION_PTR) close);
//    registerSyscall(__NR_WAITPID, (SYSCALL_FUNCTION_PTR) waitpid);
    registerSyscall(__NR_EXECVE, (SYSCALL_FUNCTION_PTR) execve);
    registerSyscall(__NR_CHDIR, (SYSCALL_FUNCTION_PTR) chdir);
//    registerSyscall(__NR_TIME, (SYSCALL_FUNCTION_PTR) time);
    registerSyscall(__NR_CHMOD, (SYSCALL_FUNCTION_PTR) chmod);
    registerSyscall(__NR_LCHOWN, (SYSCALL_FUNCTION_PTR) lchown);
//    registerSyscall(__NR_STAT, (SYSCALL_FUNCTION_PTR) stat);
    registerSyscall(__NR_LSEEK, (SYSCALL_FUNCTION_PTR) lseek);
//    registerSyscall(__NR_GETPID, (SYSCALL_FUNCTION_PTR) getpid);
//    registerSyscall(__NR_MOUNT, (SYSCALL_FUNCTION_PTR) mount);
//    registerSyscall(__NR_UMOUNT, (SYSCALL_FUNCTION_PTR) umount);
//    registerSyscall(__NR_SETUID, (SYSCALL_FUNCTION_PTR) setuid);
//    registerSyscall(__NR_GETUID, (SYSCALL_FUNCTION_PTR) getuid);
//    registerSyscall(__NR_FSTAT, (SYSCALL_FUNCTION_PTR) fstat);
//    registerSyscall(__NR_KILL, (SYSCALL_FUNCTION_PTR) kill);
//    registerSyscall(__NR_BRK, (SYSCALL_FUNCTION_PTR) brk);
//    registerSyscall(__NR_SETGID, (SYSCALL_FUNCTION_PTR) setgid);
//    registerSyscall(__NR_GETGID, (SYSCALL_FUNCTION_PTR) getgid);
    registerSyscall(__NR_IOCTL, (SYSCALL_FUNCTION_PTR) ioctl);
//    registerSyscall(__NR_FCNTL, (SYSCALL_FUNCTION_PTR) fcntl);
//    registerSyscall(__NR_GETPPID, (SYSCALL_FUNCTION_PTR) getppid);
//    registerSyscall(__NR_SETHOSTNAME, (SYSCALL_FUNCTION_PTR) sethostname);
//    registerSyscall(__NR_MMAP, (SYSCALL_FUNCTION_PTR) mmap);
//    registerSyscall(__NR_TRUNCATE, (SYSCALL_FUNCTION_PTR) truncate);
//    registerSyscall(__NR_FTRUNCATE, (SYSCALL_FUNCTION_PTR) ftruncate);
    registerSyscall(__NR_FCHMOD, (SYSCALL_FUNCTION_PTR) fchmod);
    registerSyscall(__NR_FCHOWN, (SYSCALL_FUNCTION_PTR) fchown);
    registerSyscall(__NR_LSTAT, (SYSCALL_FUNCTION_PTR) lstat);
    registerSyscall(__NR_READLINK, (SYSCALL_FUNCTION_PTR) readlink);
//    registerSyscall(__NR_FSYNC, (SYSCALL_FUNCTION_PTR) fsync);
//    registerSyscall(__NR_SETDOMAINNAME, (SYSCALL_FUNCTION_PTR) setdomainname);
    registerSyscall(__NR_UNAME, (SYSCALL_FUNCTION_PTR) Uname);
    registerSyscall(__NR_GETDENTS, (SYSCALL_FUNCTION_PTR) getdents);
//    registerSyscall(__NR_FDATASYNC, (SYSCALL_FUNCTION_PTR) fdatasync);
//    registerSyscall(__NR_PREAD, (SYSCALL_FUNCTION_PTR) pread);
//    registerSyscall(__NR_PWRITE, (SYSCALL_FUNCTION_PTR) pwrite);
    registerSyscall(__NR_CHOWN, (SYSCALL_FUNCTION_PTR) chown);
    registerSyscall(__NR_GETCWD, (SYSCALL_FUNCTION_PTR) getcwd);
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
