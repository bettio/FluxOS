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
#include <core/system.h>
#include <filesystem/vfs.h>
#include <filesystem/fscalls.h>
#include <task/task.h>
#include <arch/mips/core/cpuregistersframe.h>
#include <mm/memcalls.h>
#include <net/netcalls.h>
#include <task/archthreadsmanager.h>
#include <task/userprocessimage.h>
#include <task/userprocsmanager.h>

#define SYSCALL_FUNCTION_PTR unsigned long (*)(unsigned long, unsigned long, unsigned long)
#define SYSCALLTABLE_SIZE 256

unsigned long (*syscallsTable[SYSCALLTABLE_SIZE])(unsigned long, unsigned long, unsigned long);

void SyscallsManager::init()
{
    registerDefaultSyscalls();
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

void SyscallsManager::registerDefaultSyscalls()
{
//    registerSyscall(__NR_EXIT, (void *)  exit);
    registerSyscall(__NR_FORK, (void *)  UserProcsManager::fork);
    registerSyscall(__NR_READ, (void *)  read);
    registerSyscall(__NR_WRITE, (void *)  write);
    registerSyscall(__NR_OPEN, (void *)  open);
//    registerSyscall(__NR_CLOSE, (void *)  close);
//    registerSyscall(__NR_WAITPID, (void *)  waitpid);
    registerSyscall(__NR_EXECVE, (void *)  UserProcessImage::execve);
    registerSyscall(__NR_CHDIR, (void *)  chdir);
//    registerSyscall(__NR_TIME, (void *)  time);
    registerSyscall(__NR_CHMOD, (void *)  chmod);
    registerSyscall(__NR_LCHOWN, (void *)  lchown);
//    registerSyscall(__NR_STAT, (void *)  stat);
    registerSyscall(__NR_LSEEK, (void *)  lseek);
//    registerSyscall(__NR_GETPID, (void *)  getpid);
//    registerSyscall(__NR_MOUNT, (void *)  mount);
//    registerSyscall(__NR_UMOUNT, (void *)  umount);
//    registerSyscall(__NR_SETUID, (void *)  setuid);
//    registerSyscall(__NR_GETUID, (void *)  getuid);
//    registerSyscall(__NR_FSTAT, (void *)  fstat);
//    registerSyscall(__NR_KILL, (void *)  kill);
    registerSyscall(__NR_BRK, (void *)  brk);
//    registerSyscall(__NR_SETGID, (void *)  setgid);
//    registerSyscall(__NR_GETGID, (void *)  getgid);
    registerSyscall(__NR_IOCTL, (void *)  ioctl);
//    registerSyscall(__NR_FCNTL, (void *)  fcntl);
//    registerSyscall(__NR_GETPPID, (void *)  getppid);
//    registerSyscall(__NR_SETHOSTNAME, (void *)  sethostname);
//    registerSyscall(__NR_MMAP, (void *)  mmap);
//    registerSyscall(__NR_TRUNCATE, (void *)  truncate);
//    registerSyscall(__NR_FTRUNCATE, (void *)  ftruncate);
    registerSyscall(__NR_FCHMOD, (void *)  fchmod);
    registerSyscall(__NR_FCHOWN, (void *)  fchown);
    registerSyscall(__NR_LSTAT, (void *)  lstat);
    registerSyscall(__NR_READLINK, (void *)  readlink);
//    registerSyscall(__NR_FSYNC, (void *)  fsync);
//    registerSyscall(__NR_SETDOMAINNAME, (void *)  setdomainname);
    registerSyscall(__NR_UNAME, (void *)  Uname);
    registerSyscall(__NR_GETDENTS, (void *)  getdents);
//    registerSyscall(__NR_FDATASYNC, (void *)  fdatasync);
//    registerSyscall(__NR_PREAD, (void *)  pread);
//    registerSyscall(__NR_PWRITE, (void *)  pwrite);
    registerSyscall(__NR_SOCKET, (void *)  socket);
    registerSyscall(__NR_CHOWN, (void *)  chown);
    registerSyscall(__NR_GETCWD, (void *)  getcwd);
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
