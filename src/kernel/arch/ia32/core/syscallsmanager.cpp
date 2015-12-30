/***************************************************************************
 *   Copyright 2011 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 09/07/2005                                                      *
 ***************************************************************************/

#include <core/syscallsmanager.h>
#include <task/scheduler.h>

#include <arch/ia32/core/idt.h>
#include <core/printk.h>
#include <filesystem/vfs.h>
#include <drivers/chardevice.h>

#include <core/archmanager.h>
#include <core/systemtimer.h>
#include <core/printk.h>
#include <filesystem/vfs.h>
#include <cstdlib.h>
#include <drivers/chardevicemanager.h>
#include <core/system.h>
#include <task/task.h>
#include <filesystem/fscalls.h>
#include <filesystem/pollfd.h>
#include <net/netcalls.h>

#include <uapi/memoryuapi.h>
#include <uapi/processuapi.h>
#include <uapi/socketsyscalls.h>

#include <arch/ia32/core/userprocsmanager.h>
#include <task/scheduler.h>
#include <task/processcontrolblock.h>
#include <task/task.h>
#include <task/archthreadsmanager.h>
#include <task/userprocessimage.h>

#define SYSCALLTABLE_SIZE 256
#define IA32_SYSCALL_TYPE uint32_t (*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)

extern "C" void syscallHandler();
uint32_t (*syscallsTable[SYSCALLTABLE_SIZE])(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);

void SyscallsManager::init()
{
    IDT::setHandler(syscallHandler, 128, 3);
    registerDefaultSyscalls();
}

void SyscallsManager::registerSyscall(int num, void *funcPtr)
{
    syscallsTable[num] = (IA32_SYSCALL_TYPE) funcPtr;
}

void SyscallsManager::unregisterSyscall(int num)
{
    syscallsTable[num] = NULL;
}
    
extern "C" unsigned long doSyscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t esp)
{
    //TODO: remove this workaround: add esp parameter to all the syscalls
    if (eax == 2){
        return UserProcsManager::fork((void *) esp);

    } else if ((eax < SYSCALLTABLE_SIZE) && syscallsTable[eax]) {
            return syscallsTable[eax](ebx, ecx, edx, esi, edi);

    } else {
        printk("Invalid syscall number %i. Registers: ebx: 0x%x, ecx: 0x%x, edx: 0x%x, esi: 0x%x, edi: 0x%x, esp: 0x%x.\n",
               eax, ebx, ecx, edx, esi, edi, esp);
        return -EFAULT;
    }
}

asm(
    ".globl syscallHandler\n"
    "syscallHandler:\n"
    "cli\n"
    "pusha\n"
    "pushl %esp\n"
    "pushl %edi\n"
    "pushl %esi\n"
    "pushl %edx\n"
    "pushl %ecx\n"
    "pushl %ebx\n"
    "pushl %eax\n"
    "sti\n"
    "call doSyscall\n"
    "cli\n"
    "addl $28, %esp\n"
    "movl %eax, 28(%esp)\n"
    "popa\n"
    "sti\n"
    "iret\n"
);

uint32_t socketcall(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    int retval;
    long *args = (long *) ecx;

    switch (ebx){
        case SYS_SOCKET:
            retval = socket(args[0], args[1], args[2]);
            break;
        
        case SYS_BIND:
            retval = bind(args[0], (const sockaddr *) args[1], args[2]);
            break;

        case SYS_CONNECT:
            retval = connect(args[0], (const sockaddr *) args[1], args[2]);
            break;

        case SYS_SEND:
            retval = send(args[0], (const void *) args[1], args[2], args[3]);
            break;

        case SYS_SENDTO:
            retval = sendto(args[0], (const void *) args[1], args[2], args[3], (const sockaddr *) args[4], args[5]);
            break;

        default:
            retval = -EINVAL;
    }

    return retval;
}

void SyscallsManager::registerDefaultSyscalls()
{
    registerSyscall(1, (void *) ProcessUAPI::exit);
    registerSyscall(3, (void *) read);
    registerSyscall(4, (void *) write);
    registerSyscall(5, (void *) open);
    registerSyscall(6, (void *) close);
    registerSyscall(7, (void *) ProcessUAPI::waitpid);
    registerSyscall(8, (void *) creat);
    registerSyscall(9, (void *) link);
    registerSyscall(10, (void *) unlink);
    registerSyscall(11, (void *) UserProcessImage::execve);
    registerSyscall(12, (void *) chdir);
    registerSyscall(13, (void *) SystemTimer::time);
    registerSyscall(14, (void *) mknod);
    registerSyscall(15, (void *) chmod);
    registerSyscall(16, (void *) lchown);
    registerSyscall(18, (void *) stat);
    registerSyscall(19, (void *) lseek);
    registerSyscall(20, (void *) ProcessUAPI::getpid);
    registerSyscall(21, (void *) FileSystem::VFS::Mount);
    registerSyscall(22, (void *) FileSystem::VFS::Umount);
    registerSyscall(23, (void *) ProcessUAPI::setuid);
    registerSyscall(24, (void *) ProcessUAPI::getuid);
    registerSyscall(25, (void *) SystemTimer::stime);
    //26 ptrace
    //27 alarm
    registerSyscall(28, (void *) fstat);
    //29 pause
    registerSyscall(30, (void *) utime);
    registerSyscall(33, (void *) access);
    //34 nice
    //36 sync
    registerSyscall(37, (void *) ProcessUAPI::kill);
    registerSyscall(38, (void *) rename);
    registerSyscall(39, (void *) mkdir);
    registerSyscall(40, (void *) rmdir);
    registerSyscall(41, (void *) dup);
    registerSyscall(42, (void *) pipe);
    registerSyscall(45, (void *) MemoryUAPI::brk);
    registerSyscall(46, (void *) ProcessUAPI::setgid);
    registerSyscall(47, (void *) ProcessUAPI::getgid);
    //48 sys_signal
    //49 geteuid
    //50 getegid
    //52 umount2
    registerSyscall(54, (void *) ioctl);
    registerSyscall(55, (void *) fcntl);
    //57 setpgid
    registerSyscall(60, (void *) umask);
    //61 chroot
    //62 ustat
    registerSyscall(32, (void *) dup2);
    registerSyscall(64, (void *) ProcessUAPI::getppid);
    //65 getpgrp
    //66 setsid
    //67 sigaction
    //68 sgetmask
    //69 ssetmask
    //70 setreuid
    //71 setregid
    //72 sigsuspend
    //73 sigpending
    registerSyscall(74, (void *) SetHostName);
    //gettimeofday 78
    //settimeofday 79
    //getgroups 80
    //setgroups 81
    registerSyscall(83, (void *) symlink);
    registerSyscall(84, (void *) lstat);
    registerSyscall(85, (void *) readlink);
    //86 uselib
    registerSyscall(88, (void *) ArchManager::reboot);
    registerSyscall(90, (void *) mmap_i386);
    registerSyscall(91, (void *) MemoryUAPI::munmap);
    registerSyscall(92, (void *) truncate);
    registerSyscall(93, (void *) ftruncate);
    registerSyscall(94, (void *) fchmod);
    registerSyscall(95, (void *) fchown);
    //96 getpriority
    //97 setpriority
    registerSyscall(99, (void *) statfs);
    registerSyscall(100, (void *) fstatfs);
    registerSyscall(102, (void *) socketcall);
    //104 setitimer
    //105 getitimer
    registerSyscall(107, (void *) lstat);
    //111 vhangup
    //114 wait4
    //116 sysinfo
    registerSyscall(118, (void *) fsync);
    //119 sigreturn
    //120 clone
    registerSyscall(121, (void *) SetDomainName);
    registerSyscall(122, (void *) Uname);
    registerSyscall(125, (void *) MemoryUAPI::mprotect);
    //126 sigprocmask
    //132 getpgid
    registerSyscall(133, (void *) fchdir);
    //138 setfsuid
    //139 setfsgid 
    registerSyscall(141, (void *) getdents);
    //142 select
    //143 flock
    registerSyscall(144, (void *) MemoryUAPI::msync);
    //145 readv
    //146 writev
    //147 sys_getsid
    registerSyscall(148, (void *) fdatasync);
    //149 sysctl
    registerSyscall(150, (void *) MemoryUAPI::mlock);
    registerSyscall(151, (void *) MemoryUAPI::munlock);
    registerSyscall(152, (void *) MemoryUAPI::mlockall);
    registerSyscall(153, (void *) MemoryUAPI::munlockall);
    //162 nanosleep
    registerSyscall(163, (void *) MemoryUAPI::mremap);
    //165 getresuid
    registerSyscall(168, (void *) poll);
    //170 setresid
    //171 getresid
    //172 prctl
    registerSyscall(180, (void *) pread);
    registerSyscall(181, (void *) pwrite);
    registerSyscall(182, (void *) chown);
    registerSyscall(183, (void *) getcwd);
    //184 capget
    //185 capset
    //186 sigaltstack
    //187 sendfile
    //190 vfork
    //295 openat
}

