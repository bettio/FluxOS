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

#include <arch/ia32/core/syscallsmanager.h>
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

#include <arch/ia32/core/userprocsmanager.h>
#include <task/scheduler.h>
#include <task/processcontrolblock.h>
#include <task/task.h>
#include <task/archthreadsmanager.h>

#include <mm/memcalls.h>

#define SYSCALL_MAXNUM 256

struct MmapArgs
{
	unsigned long addr;
	unsigned long len;
	unsigned long prot;
	unsigned long flags;
	unsigned long fd;
	unsigned long offset;
};

extern "C" void syscallHandler();

uint32_t (*SyscallsTable[SYSCALL_MAXNUM])(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);

void SyscallsManager::init()
{
    IDT::setHandler(syscallHandler, 128, 3);

    for(int i = 0; i < SYSCALL_MAXNUM; i++){
        registerSyscall(i, nullSysCallHandler);
    }
    registerDefaultSyscalls();
}

void SyscallsManager::registerSyscall(int num, uint32_t (*func)(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi))
{
    SyscallsTable[num] = func;
}

void SyscallsManager::unregisterSyscall(int num)
{
    SyscallsTable[num] = nullSysCallHandler;
}

uint32_t SyscallsManager::nullSysCallHandler(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    printk("Null System Call Handler:\n"
           " - ebx: 0x%x, ecx: 0x%x\n"
           " - edx: 0x%x, esi: 0x%x\n"
           " - edi: 0x%x\n",
           ebx, ecx, edx, esi, edi);

    return -EFAULT;
}
    
extern "C" uint32_t doSyscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t esp)
{
    //TODO: remove this workaround: add esp parameter to all the syscalls
    if (eax == 2){
        return UserProcsManager::fork((void *) esp);

    }else if (eax < 256){
        return SyscallsTable[eax](ebx, ecx, edx, esi, edi);
    
    }else{
         return SyscallsManager::nullSysCallHandler(ebx, ecx, edx, esi, edi);
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

uint32_t exit(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    Task::exit(ebx);
    return 0;
}

uint32_t read(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return read((int) ebx, (void *) ecx, (size_t) edx);
}

uint32_t write(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return write((int) ebx, (void *) ecx, (size_t) edx);
}

uint32_t symlink(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return symlink((const char *) ebx, (const char *) ecx);
}

uint32_t readlink(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return readlink((const char *) ebx, (char *) ecx, (size_t) edx);
}

uint32_t reboot(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t)
{
    ArchManager::reboot();
    return -EPERM;
}

uint32_t getppid(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Scheduler::currentThread()->parentProcess->parent->pid;
}

uint32_t getpid(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Scheduler::currentThread()->parentProcess->pid;
}

uint32_t getgid(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Scheduler::currentThread()->parentProcess->gid;
}

uint32_t getuid(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Scheduler::currentThread()->parentProcess->uid;
}

uint32_t stime(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return (uint32_t) SystemTimer::stime((long *) ebx);
}

uint32_t setgid(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Task::SetGid(ebx);
}

uint32_t setuid(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Task::SetUid(ebx);
}

uint32_t open(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return open((const char *) ebx, (int) ecx);
}

uint32_t close(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return close((int) ebx);
}

uint32_t execve(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return UserProcsManager::execve((char *) ebx, (char **) ecx, (char **) edx);
}

uint32_t waitpid(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    return Task::waitpid(ebx, (int *) ecx, edx);
}

uint32_t creat(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return creat((const char *) ebx, (mode_t) ecx);
}

uint32_t link(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return link((const char *) ebx, (const char *) ecx);
}

uint32_t unlink(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return unlink((const char *) ebx);
}

uint32_t getcwd(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return getcwd((char *) ebx, (int) ecx);
}

uint32_t mount(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    return FileSystem::VFS::Mount((const char *) ebx, (const char *) ecx, (const char *) edx, (unsigned int) esi, (const void *) edi);
}

uint32_t umount(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return FileSystem::VFS::Umount((const char *) ebx);
}

uint32_t kill(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    printk("Not implemented syscall: kill\n");
    while(1);

    return 0;
}

uint32_t rename(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return rename((const char *) ebx, (const char *) ecx);
}

uint32_t mkdir(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return mkdir((const char *) ebx, (mode_t) ecx);
}

uint32_t rmdir(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return rmdir((const char *) ebx);
}

uint32_t dup(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return dup((int) ebx);
}

uint32_t pipe(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return pipe((int *) ebx);
}

uint32_t brk(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return (uint32_t) brk((void *) ebx);
}

uint32_t chdir(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return chdir((const char *) ebx);
}

uint32_t getdents(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return getdents((unsigned int) ebx, (dirent *) ecx, (unsigned int) edx);
}

uint32_t time(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return SystemTimer::time((long *) ebx);
}

uint32_t mknod(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return mknod((const char *) ebx, (mode_t) ecx, (dev_t) edx);
}

uint32_t sethostname(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return SetHostName((const char *) ebx, (size_t) ecx);   
}

uint32_t setdomainname(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return SetDomainName((const char *) ebx, (size_t) ecx);
}

uint32_t lseek(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return lseek((int) ebx, (off_t) ecx, (int) edx);
}

uint32_t uname(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Uname((utsname *) ebx);
}

uint32_t stat(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return stat((const char *) ebx, (struct stat *) ecx);
}

uint32_t lstat(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return lstat((const char *) ebx, (struct stat *) ecx);
}

uint32_t fstat(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return fstat((int) ebx, (struct stat *) ecx);
}

uint32_t fsync(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return fsync((int) ebx);
}

uint32_t fdatasync(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return fdatasync((int) ebx);
}

uint32_t truncate(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return truncate((const char *) ebx, (uint64_t) ecx);
}

uint32_t ftruncate(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return ftruncate((int) ebx, (uint64_t) ecx);
}

uint32_t chmod(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return chmod((const char *) ebx, (mode_t) ecx);
}

uint32_t fchmod(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    return fchmod((int) ebx, (mode_t) ecx);
}

uint32_t chown(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return chown((const char *) ebx, (uid_t) ecx, (gid_t) edx);
}

uint32_t fchown(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return fchown((int) ebx, (uid_t) ecx, (gid_t) edx);
}

uint32_t lchown(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return lchown((const char *) ebx, (uid_t) ecx, (gid_t) edx);
}

uint32_t ioctl(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    return ioctl((int) ebx, (int) ecx, (long) edx);
}

uint32_t fcntl(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    return fcntl((int) ebx, (int) ecx, (long) edx);
}

uint32_t pread(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    return pread((int) ebx, (void *) ecx, (size_t) edx, (uint64_t) esi);
}

uint32_t pwrite(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    return pwrite((int) ebx, (void *) ecx, (size_t) edx, (uint64_t) esi);
}

uint32_t mmap(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    MmapArgs *args = (MmapArgs *) ebx;
    return (uint32_t) mmap((void *) args->addr, (size_t) args->len, (int) args->prot, (int) args->flags, (int) args->fd, (size_t) args->offset);
}

void SyscallsManager::registerDefaultSyscalls()
{
    registerSyscall(1, exit);
    registerSyscall(3, read);
    registerSyscall(4, write);
    registerSyscall(5, open);
    registerSyscall(6, close);
    registerSyscall(7, waitpid);
    registerSyscall(8, creat);
    registerSyscall(9, link);
    registerSyscall(10, unlink);
    registerSyscall(11, execve);
    registerSyscall(12, chdir);
    registerSyscall(13, time);
    registerSyscall(14, mknod);
    registerSyscall(15, chmod);
    registerSyscall(16, lchown);
    registerSyscall(18, stat);
    registerSyscall(19, lseek);
    registerSyscall(20, getpid);
    registerSyscall(21, mount);
    registerSyscall(22, umount);
    registerSyscall(23, setuid);
    registerSyscall(24, getuid);
    registerSyscall(25, stime);
    registerSyscall(28, fstat);
    //30 utime
    //33 access
    //34 nice
    registerSyscall(37, kill);
    registerSyscall(38, rename);
    registerSyscall(39, mkdir);
    registerSyscall(40, rmdir);
    registerSyscall(41, dup);
    registerSyscall(42, pipe);
    registerSyscall(45, brk);
    registerSyscall(46, setgid);
    registerSyscall(47, getgid);
    //52 umount2
    registerSyscall(54, ioctl);
    registerSyscall(55, fcntl);
    //57 setpgid
    //60 umask
    //61 ustat
    //63 dup2
    registerSyscall(64, getppid);
    registerSyscall(74, sethostname);
    registerSyscall(83, symlink);
    registerSyscall(85, readlink);
    registerSyscall(88, reboot);
    registerSyscall(90, mmap);
    registerSyscall(92, truncate);
    registerSyscall(93, ftruncate);
    registerSyscall(94, fchmod);
    registerSyscall(95, fchown);
    registerSyscall(107, lstat);
    registerSyscall(118, fsync);
    registerSyscall(121, setdomainname);
    registerSyscall(122, uname);
    registerSyscall(141, getdents);
    registerSyscall(148, fdatasync);
    registerSyscall(180, pread);
    registerSyscall(181, pwrite);
    registerSyscall(182, chown);
    registerSyscall(183, getcwd);
    //187 sendfile
}

