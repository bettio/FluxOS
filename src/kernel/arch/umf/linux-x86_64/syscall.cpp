/***************************************************************************
 *   Copyright 2008 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: syscalls.cpp                                                    *
 *   Date: 16/07/2008                                                      *
 ***************************************************************************/

#include <task/task.h>
#include <arch/umf/core/hostsyscalls.h>

#include <core/printk.h>
#include <filesystem/vfs.h>
#include <cstdlib.h>
#include <drivers/chardevicemanager.h>
#include <core/system.h>
#include <filesystem/fscalls.h>

#include <lib/koof/intkeymap.h>

#include <cstring.h>

#include <arch/umf/core/kmemcpy.h>
#include <arch/umf/linux-x86_64/syscall.h>

#define TMP_BUF_SIZE 4096
#define TMP_FAST_BUF_SIZE 256

//HACK: this is an ugly hack, I should do something better
extern int cpid;
extern IntKeyMap<int> *Pids;

uint64_t (*SysCall::SysCallTable[256])(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi);

uint64_t write(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char tmpFastBuf[TMP_FAST_BUF_SIZE];
	void *tmpBuf = (edx <= TMP_FAST_BUF_SIZE) ? (void *) tmpFastBuf : malloc(edx);

	MemCpyFromUserToKernel(tmpBuf, (void *) ecx, edx, cpid);

	long retVal = write((int) ebx, tmpBuf, edx);

	free(tmpBuf);

	return retVal;
}

uint64_t readlink(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	char *tmpBufLink = (char *) malloc(edx);

	StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = readlink(tmpBufPath, tmpBufLink, (size_t) edx);

	MemCpyFromKernelToUser((void *) ecx, tmpBufLink, edx, cpid);

	free(tmpBufPath);
	free(tmpBufLink);

	return retVal;
}

uint64_t getcwd(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBuf = (char *) malloc(ecx);

	long retVal = getcwd(tmpBuf, (int) ecx);

	MemCpyFromKernelToUser((void *) ebx, tmpBuf, ecx, cpid);

	free(tmpBuf);

	return retVal;
}

uint64_t time(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	if (ebx == 0){
		return HostSysCalls::time(0);

	}else{
		unsigned long tmpBuf;

		long retVal = (uint64_t) HostSysCalls::time(&tmpBuf);

		MemCpyFromKernelToUser((void *) ebx, &tmpBuf, sizeof(unsigned long), cpid);

		return retVal;
	}
}

uint64_t sethostname(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBuf = (char *) malloc(edx);

	MemCpyFromUserToKernel(tmpBuf, (void *) ecx, edx, cpid);

	long retVal = (long) SetHostName(tmpBuf, (size_t) ecx);

	free(tmpBuf);

	return retVal;
}

uint64_t setdomainname(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBuf = (char *) malloc(edx);

	MemCpyFromUserToKernel(tmpBuf, (void *) ecx, edx, cpid);

	long retVal = SetDomainName(tmpBuf, (size_t) ecx);

	free(tmpBuf);

	return retVal;
}

uint64_t uname(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
    printk("Here I am 2: %i, %i, %i, %i, %i\n", ebx, ecx, edx, esi, edi);

	utsname tmpBuf;

	long retVal = Uname(&tmpBuf);

	MemCpyFromKernelToUser((void *) ebx, &tmpBuf, sizeof(utsname), cpid);

	return retVal;
}

uint64_t getppid(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return Task::CurrentTask()->Parent->Pid;
}

uint64_t getpid(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return Task::CurrentTask()->Pid;
}

uint64_t getgid(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return Task::CurrentTask()->Gid;
}

uint64_t getuid(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return Task::CurrentTask()->Uid;
}

uint64_t setgid(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return (uint64_t) Task::SetGid(ebx);
}

uint64_t setuid(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return (uint64_t) Task::SetUid(ebx);
}

uint64_t kill(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	printk("Unimplemented syscall: kill\n");
	while(1);

	return 0;
}

uint64_t close(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return close((int) ebx);
}

uint64_t chdir(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBuf = (char *) malloc(TMP_FAST_BUF_SIZE);

	StrNCpyFromUserToKernel(tmpBuf, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = chdir((const char *) tmpBuf);

	free(tmpBuf);

	return retVal;
}

uint64_t open(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBuf = (char *) malloc(TMP_FAST_BUF_SIZE);

	StrNCpyFromUserToKernel(tmpBuf, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = open((const char *) tmpBuf, (int) ecx);

	free(tmpBuf);

	return retVal;
}

uint64_t creat(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
    char *tmpBuf = (char *) malloc(TMP_FAST_BUF_SIZE);

    StrNCpyFromUserToKernel(tmpBuf, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

    long retVal = creat((const char *) tmpBuf, (mode_t) ecx);

    free(tmpBuf);

    return retVal;
}
uint64_t mount(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBufSource = (char *) malloc(TMP_FAST_BUF_SIZE);
	char *tmpBufTarget = (char *) malloc(TMP_FAST_BUF_SIZE);
	char *tmpBufFSType = (char *) malloc(TMP_FAST_BUF_SIZE);

	StrNCpyFromUserToKernel(tmpBufSource, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);
	StrNCpyFromUserToKernel(tmpBufTarget, (const char *) ecx, TMP_FAST_BUF_SIZE, cpid);
	StrNCpyFromUserToKernel(tmpBufFSType, (const char *) edx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = FileSystem::VFS::Mount(tmpBufSource, tmpBufTarget, tmpBufFSType, 0, 0);

	free(tmpBufSource);
	free(tmpBufTarget);
	free(tmpBufFSType);

	return retVal;
}

//TODO flags
uint64_t umount(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBuf = (char *) malloc(TMP_FAST_BUF_SIZE);

	StrNCpyFromUserToKernel(tmpBuf, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = FileSystem::VFS::Umount(tmpBuf);

	free(tmpBuf);

	return retVal;
}

uint64_t stat(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t esi, uint64_t edi)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	struct stat tmpBufStat;

	StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = stat(tmpBufPath, &tmpBufStat);

	MemCpyFromKernelToUser((void *) ecx, &tmpBufStat, sizeof(struct stat), cpid);

	free(tmpBufPath);

	return retVal;
}

uint64_t lstat(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t esi, uint64_t edi)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	struct stat tmpBufStat;

    StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = stat(tmpBufPath, &tmpBufStat);

	MemCpyFromKernelToUser((void *) ecx, &tmpBufStat, sizeof(struct stat), cpid);

	free(tmpBufPath);

	return retVal;
}

uint64_t fstat(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t esi, uint64_t edi)
{
	struct stat tmpBuf;

	long retVal = fstat((int) ebx, &tmpBuf);

	MemCpyFromKernelToUser((void *) ecx, &tmpBuf, sizeof(struct stat), cpid);

	return retVal;
}

uint64_t lseek(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return lseek((int) ebx, (off_t) ecx, (int) edx);
}

uint64_t getdents(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	dirent *tmpBuf = (dirent *) malloc(edx);

	long retVal = getdents((unsigned int) ebx, tmpBuf, (unsigned int) edx);

	MemCpyFromKernelToUser((void *) ecx, tmpBuf, edx, cpid);

	free(tmpBuf);

	return retVal;
}

uint64_t read(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char tmpFastBuf[TMP_FAST_BUF_SIZE];
	void *tmpBuf = (edx <= TMP_FAST_BUF_SIZE) ? (void *) tmpFastBuf : malloc(edx);

	long retVal;

	retVal = read((int) ebx, tmpBuf, (size_t) edx);

	MemCpyFromKernelToUser((void *) ecx, tmpBuf, edx, cpid);

	return retVal;
}

//TODO: fork
uint64_t fork(uint64_t, uint64_t, uint64_t, uint64_t esi, uint64_t edi){
	printk("Fork not implemented\n");
	while(1);

	return 0;
}

//TODO: execve
uint64_t execve(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	printk("Unimplemented syscall: execve\n");
	while(1);

	return 0;
}

uint64_t waitpid(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
    while (Task::TaskDescriptorTable[ebx].Status != TERMINATED);

	return 0;
}

uint64_t fsync(uint64_t ebx, uint64_t, uint64_t, uint64_t, uint64_t)
{
	return fsync((int) ebx);
}

uint64_t fdatasync(uint64_t ebx, uint64_t, uint64_t, uint64_t, uint64_t)
{
	return fdatasync((int) ebx);
}

uint64_t truncate(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	if (tmpBufPath == NULL) return -ENOMEM;
	StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = truncate(tmpBufPath, (uint64_t) ecx);

	free(tmpBufPath);

	return retVal;
}

uint64_t ftruncate(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
	return ftruncate((int) ebx, (uint64_t) ecx);
}

uint64_t chmod(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	if (tmpBufPath == NULL) return -ENOMEM;
	StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = chmod(tmpBufPath, (mode_t) ecx);

	free(tmpBufPath);

	return retVal;
}

uint64_t fchmod(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
	return fchmod((int) ebx, (mode_t) ecx);
}

uint64_t chown(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t, uint64_t)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	if (tmpBufPath == NULL) return -ENOMEM;
	StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = chown(tmpBufPath, (uid_t) ecx, (gid_t) edx);

	free(tmpBufPath);

	return retVal;
}

uint64_t fchown(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t, uint64_t)
{
	return fchown((int) ebx, (uid_t) ecx, (gid_t) edx);
}

uint64_t lchown(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	if (tmpBufPath == NULL) return -ENOMEM;
	StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	int ret = lchown(tmpBufPath, (uid_t) ecx, (gid_t) edx);

	free(tmpBufPath);

	return ret;
}

uint64_t ioctl(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t, uint64_t)
{
	return ioctl((int) ebx, (int) ecx, (long) edx);
}

uint64_t fcntl(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t, uint64_t)
{
	return fcntl((int) ebx, (int) ecx, (long) edx);
}

//TODO: pwrite
uint64_t pwrite(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return 0;
}

//TODO: pread
uint64_t pread(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	return 0;
}

uint64_t stat64(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	struct stat64 tmpBufStat;

	StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	long retVal = stat64(tmpBufPath, &tmpBufStat);

	MemCpyFromKernelToUser((void *) ecx, &tmpBufStat, sizeof(struct stat64), cpid);

	free(tmpBufPath);

	return retVal;
}

uint64_t lstat64(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
	char *tmpBufPath = (char *) malloc(TMP_FAST_BUF_SIZE);
	struct stat64 tmpBufStat;

	StrNCpyFromUserToKernel(tmpBufPath, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

	uint64_t ret = lstat64(tmpBufPath, &tmpBufStat);

	MemCpyFromKernelToUser((void *) ecx, &tmpBufStat, sizeof(struct stat64), cpid);

	free(tmpBufPath);

	return ret;
}

uint64_t fstat64(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
	struct stat64 tmpBuf;

	long retVal = fstat64((int) ebx, &tmpBuf);

	MemCpyFromKernelToUser((void *) ecx, &tmpBuf, sizeof(struct stat64), cpid);

	return retVal;
}

void CreateNewProcess(char *tmpProcName, char *tmpProcParams);



uint64_t CreateProcess(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
    char *tmpProcName = (char *) malloc(TMP_FAST_BUF_SIZE);
    StrNCpyFromUserToKernel(tmpProcName, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);
    char *tmpProcParams = (char *) malloc(TMP_FAST_BUF_SIZE);
    StrNCpyFromUserToKernel(tmpProcParams, (const char *) ecx, TMP_FAST_BUF_SIZE, cpid);
    
    int newPid = Task::MaxUsedTaskPid();
    int oldPidsSize = Pids->Size();
    
    HostSysCalls::newThread((int (*)(void*)) CreateNewProcess, tmpProcName, tmpProcParams);

    ///printk("New pid: %i\n", newPid);
    
    while(oldPidsSize == Pids->Size());

    Task::TaskDescriptorTable[newPid].CwdNode = Task::CurrentTask()->CwdNode;

    return newPid;
}

uint64_t mkdir(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
    char *tmpBuf = (char *) malloc(TMP_FAST_BUF_SIZE);

    StrNCpyFromUserToKernel(tmpBuf, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

    long retVal = mkdir((const char *) tmpBuf, (mode_t) ecx);

    free(tmpBuf);

    return retVal;
}

uint64_t rmdir(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
    char *tmpBuf = (char *) malloc(TMP_FAST_BUF_SIZE);

    StrNCpyFromUserToKernel(tmpBuf, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

    long retVal = rmdir((const char *) tmpBuf);

    free(tmpBuf);

    return retVal;
}

uint64_t unlink(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
    char *tmpBuf = (char *) malloc(TMP_FAST_BUF_SIZE);

    StrNCpyFromUserToKernel(tmpBuf, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);

    long retVal = unlink((const char *) tmpBuf);

    free(tmpBuf);

    return retVal;
}

uint64_t symlink(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
    char *tmpBuf0 = (char *) malloc(TMP_FAST_BUF_SIZE);
    char *tmpBuf1 = (char *) malloc(TMP_FAST_BUF_SIZE);
    
    StrNCpyFromUserToKernel(tmpBuf0, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);
    StrNCpyFromUserToKernel(tmpBuf1, (const char *) ecx, TMP_FAST_BUF_SIZE, cpid);
    
    long retVal = symlink(tmpBuf0, tmpBuf1);

    free(tmpBuf0);
    free(tmpBuf1);

    return retVal;
}

uint64_t link(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
    char *tmpBuf0 = (char *) malloc(TMP_FAST_BUF_SIZE);
    char *tmpBuf1 = (char *) malloc(TMP_FAST_BUF_SIZE);
    
    StrNCpyFromUserToKernel(tmpBuf0, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);
    StrNCpyFromUserToKernel(tmpBuf1, (const char *) ecx, TMP_FAST_BUF_SIZE, cpid);
    
    long retVal = link(tmpBuf0, tmpBuf1);

    free(tmpBuf0);
    free(tmpBuf1);

    return retVal;
}

uint64_t rename(uint64_t ebx, uint64_t ecx, uint64_t, uint64_t, uint64_t)
{
    char *tmpBuf0 = (char *) malloc(TMP_FAST_BUF_SIZE);
    char *tmpBuf1 = (char *) malloc(TMP_FAST_BUF_SIZE);
    
    StrNCpyFromUserToKernel(tmpBuf0, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);
    StrNCpyFromUserToKernel(tmpBuf1, (const char *) ecx, TMP_FAST_BUF_SIZE, cpid);
    
    long retVal = rename(tmpBuf0, tmpBuf1);

    free(tmpBuf0);
    free(tmpBuf1);

    return retVal;
}

uint64_t mknod(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t, uint64_t)
{
    char *tmpBuf0 = (char *) malloc(TMP_FAST_BUF_SIZE);
    
    StrNCpyFromUserToKernel(tmpBuf0, (const char *) ebx, TMP_FAST_BUF_SIZE, cpid);
    
    long retVal = mknod(tmpBuf0, (mode_t) ecx, (dev_t) edx);

    free(tmpBuf0);

    return retVal;
}

uint64_t SysCall::NullSysCallHandler(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi)
{
	printk("Null System Call Handler:\n"
		" - ebx: 0x%x, ecx: 0x%x\n"
		" - edx: 0x%x, esi: 0x%x\n"
		" - edi: 0x%x",
		ebx, ecx, edx, esi, edi);
	while(1);

	return 0; //TODO: fare qualcosa di piu` appropriato
}

void SysCall::Add(uint64_t (*func)(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi), int sysC)
{
	SysCallTable[sysC] = func;
}

void SysCall::Remove(int sysC)
{
	SysCallTable[sysC] = NullSysCallHandler;
}

void SysCall::Init()
{
	for(int i = 0; i < 256; i++) Add(NullSysCallHandler, i);

	//Add(exit, 60); //TODO: missing
	Add(fork, 57);
	Add(read, 0);
	Add(write, 1);
	Add(open, 2);
	Add(close, 3);
	Add(waitpid, 61);
	Add(execve, 59);
	Add(chdir, 80);
	Add(time, 13);
	Add(chmod, 15);
	Add(lchown, 94);
	Add(stat, 4);
	Add(lseek, 8);
	Add(getpid, 20);
	Add(mount, 165);
	Add(umount, 166);
	Add(setuid, 23);
	Add(getuid, 102);
	Add(fstat, 5);
	Add(kill, 62);
	//Add(brk, 12); //TODO: missing
	Add(setgid, 46);
	Add(getgid, 47);
	Add(ioctl, 16);
	Add(fcntl, 72);
	Add(getppid, 64);
	Add(sethostname, 74);
    Add(rename, 82);
    Add(mkdir, 83);
    Add(rmdir, 84);
    Add(link, 86);
    Add(unlink, 87);
    Add(symlink, 88);
    Add(chmod, 90);
	Add(truncate, 76);
	Add(ftruncate, 93);
	Add(fchmod, 94);
	Add(fchown, 93);
	Add(lstat, 6);
	Add(creat, 85);
	Add(fsync, 118);
	Add(setdomainname, 121);
	Add(uname, 63);
	Add(getdents, 78);
	Add(fdatasync, 148);
	Add(pread, 17);
	Add(pwrite, 18);
	Add(chown, 92);
	Add(getcwd, 79);
    Add(mknod, 133);
	Add(stat64, 195);
	Add(lstat64, 196);
	Add(fstat64, 197);
	Add(CreateProcess, 220); //TODO: missing, kill me?
}
