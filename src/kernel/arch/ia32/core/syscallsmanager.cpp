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

#include <arch/ia32/core/idt.h>
#include <core/printk.h>
#include <filesystem/vfs.h>
#include <drivers/chardevice.h>

#include <core/printk.h>
#include <filesystem/vfs.h>
#include <cstdlib.h>
#include <drivers/chardevicemanager.h>
#include <core/system.h>
#include <task/task.h>
#include <filesystem/fscalls.h>

#define SYSCALL_MAXNUM 256

extern "C" void syscallHandler();

uint32_t (*SyscallsTable[SYSCALL_MAXNUM])(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);

void SyscallsManager::init()
{
    IDT::setHandler(syscallHandler, 128, 0 /* RING_0 */); //TODO: change to ring3

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
           " - edi: 0x%x",
           ebx, ecx, edx, esi, edi);

    return -EFAULT;
}
    
extern "C" void doSyscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t esp)
{
    if (eax < 256){
        SyscallsTable[eax](ebx, ecx, edx, esi, edi);
    
    }else{
         SyscallsManager::nullSysCallHandler(ebx, ecx, edx, esi, edi);
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
    "call doSyscall\n"
    "addl $28, %esp\n"
    "popa\n"
    "sti\n"
    "iret\n"
);

uint32_t exit(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    printk("Not implemented syscall: exit\n");
    asm("sti");
    while(1);
    return 0;
}

uint32_t read(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return read((int) ebx, (void *) ecx, (size_t) edx);
}

uint32_t write(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    if (ebx == 1){
        return Out->Write(Out, (char *) ecx, (int) edx);
    }else{
        return write((int) ebx, (void *) ecx, (size_t) edx);
    }
}

uint32_t readlink(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t, uint32_t)
{
    return readlink((const char *) ebx, (char *) ecx, (size_t) edx);
}

uint32_t fork(uint32_t, uint32_t, uint32_t, uint32_t esi, uint32_t edi)
{
    printk("Not implemented syscall: fork\n");
    while(1);

    return 0;
}

uint32_t CreateProcess(uint32_t ebx, uint32_t ecx, uint32_t, uint32_t, uint32_t)
{
    printk("Not implemented syscall: CreateProcess\n");
    while(1);

    return 0;
}

uint32_t getppid(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Task::CurrentTask()->Parent->Pid;
}

uint32_t getpid(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Task::CurrentTask()->Pid;
}

uint32_t getgid(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Task::CurrentTask()->Gid;
}

uint32_t getuid(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
{
    return Task::CurrentTask()->Uid;
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
    return 0;
}

uint32_t waitpid(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    printk("Not implemented syscall: waitpid\n");
    while(1);

    return 0;
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

uint32_t brk(uint32_t ebx, uint32_t, uint32_t, uint32_t, uint32_t)
{
    printk("Not implemented syscall: brk\n");
    while(1);

    return 0;
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
    printk("Not implemented syscall: time\n");
    while(1);

    return 0;
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
    printk("Not implemented syscall: mmap\n");
    while(1);

    return 0;
}

void SyscallsManager::registerDefaultSyscalls()
{
    registerSyscall(1, exit);
    registerSyscall(2, fork);
    registerSyscall(3, read);
    registerSyscall(4, write);
    registerSyscall(5, open);
    registerSyscall(6, close);
    registerSyscall(7, waitpid);
    registerSyscall(11, execve);
    registerSyscall(12, chdir);
    registerSyscall(13, time);
    registerSyscall(15, chmod);
    registerSyscall(16, lchown);
    registerSyscall(18, stat);
    registerSyscall(19, lseek);
    registerSyscall(20, getpid);
    registerSyscall(21, mount);
    registerSyscall(22, umount);
    registerSyscall(23, setuid);
    registerSyscall(24, getuid);
    registerSyscall(28, fstat);
    registerSyscall(37, kill);
    registerSyscall(45, brk);
    registerSyscall(46, setgid);
    registerSyscall(47, getgid);
    registerSyscall(54, ioctl);
    registerSyscall(55, fcntl);
    registerSyscall(64, getppid);
    registerSyscall(74, sethostname);
    registerSyscall(90, mmap);
    registerSyscall(92, truncate);
    registerSyscall(93, ftruncate);
    registerSyscall(94, fchmod);
    registerSyscall(95, fchown);
    registerSyscall(107, lstat);
    registerSyscall(85, readlink);
    registerSyscall(118, fsync);
    registerSyscall(121, setdomainname);
    registerSyscall(122, uname);
    registerSyscall(141, getdents);
    registerSyscall(148, fdatasync);
    registerSyscall(180, pread);
    registerSyscall(181, pwrite);
    registerSyscall(182, chown);
    registerSyscall(183, getcwd);
}