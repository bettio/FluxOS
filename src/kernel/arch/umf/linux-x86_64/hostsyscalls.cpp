/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: hostsyscalls.cpp                                                *
 *   Date: 06/08/2006                                                      *
 ***************************************************************************/

#include <arch/umf/core/hostsyscalls.h>

#include <cstdlib.h>

#define SYSCALL_2(name, num, retT, arg0_t, arg1_t) \
    retT name(arg0_t arg0, arg1_t arg1) \
    { \
        register long syscall asm("%rax") = num; \
        register long _arg0 asm("%rdi") = (long) arg0; \
        register long _arg1 asm("%rsi") = (long) arg1; \
        \
        register long result asm("%rax"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1) \
                     : "%rcx", "%r11"); \
        \
        return result; \
}

SYSCALL_2(HostSysCalls::munmap, 11, int, void *, size_t)

int HostSysCalls::open(const char *pathname, int flags, int mode)
{
	//Parameters
	register long syscall asm("%rax") = 2;
	register long path asm("%rdi") = (long) pathname;
	register long open_flags asm("%rsi") = flags;
	register long open_mode asm("%rdx") = mode;

	//Result
	register long fdesc asm("%rax");

	asm volatile("syscall" : "=r" (fdesc) : "r" (syscall), "r" (path), "r" (open_flags), "r" (open_mode) : "%rcx", "%r11");

	return fdesc;
}

int HostSysCalls::creat(const char *pathname, int mode)
{
    //Parameters
    register long syscall asm("%rax") = 85;
    register long path asm("%rdi") = (long) pathname;
    register long _mode asm("%rsi") = mode;

    //Result
    register long fdesc asm("%rax");

    asm volatile("syscall" : "=r" (fdesc) : "r" (syscall), "r" (path), "r" (_mode) : "%rcx", "%r11");

    return fdesc;
}

int HostSysCalls::write(int fd, const char *buf, size_t count)
{
	//Parameters
	register long syscall asm("%rax") = 1;
	register long fdesc asm("%rdi") = fd;
	register long buffer asm("%rsi") = (long) buf;
	register long size asm("%rdx") = count;

	//Result
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (fdesc), "r" (buffer), "r" (size) : "%rcx", "%r11");

	return result;
}

int HostSysCalls::read(int fd, void *buf, size_t count)
{
	//Parameters
	register long syscall asm("%rax") = 0;
	register long fdesc asm("%rdi") = fd;
	register long buffer asm("%rsi") = (long) buf;
	register long size asm("%rdx") = count;

	//Result
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (fdesc), "r" (buffer), "r" (size) : "%rcx", "%r11");

	return result;
}

int HostSysCalls::close(int fd)
{
    //Parameters
    register long syscall asm("%rax") = 3;
    register long fdesc asm("%rdi") = fd;

    //Result
    register long result asm("%rax");

    asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (fdesc) : "%rcx", "%r11");

    return result;
}

//TODO: FIXME
int HostSysCalls::lseek(int fildes, uint64_t offset, int whence)
{
	//Parameters
	register long syscall asm("%rax") = 8;
	register long fdesc asm("%rdi") = fildes;
	register long seek_offset asm("%rsi") = (long) offset;
	register long seek_whence asm("%rdx") = whence;

	//Result
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (fdesc), "r" (seek_offset), "r" (seek_whence) : "%rcx", "%r11");

	return result;
}

long HostSysCalls::ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data)
{
	long tmpData = 0;

	//Parameters
	register long syscall asm("%rax") = 101;
	register long requestReg asm("%rdi") = request;
	register long pidReg asm("%rsi") = pid;
	register long addrReg asm("%rdx") = (long) addr;
	//TODO: Valgrind warning
	register long dataReg asm("%r10") = (request == PTRACE_PEEKDATA || request == PTRACE_PEEKTEXT || request == PTRACE_PEEKUSER)
						? (long) &tmpData : (long) data;

	//Result
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (requestReg), "r" (pidReg), "r" (addrReg), "r" (dataReg) : "%rcx", "%r11");

	//TODO: Valgrind warning
	return (request == PTRACE_PEEKDATA || request == PTRACE_PEEKTEXT || request == PTRACE_PEEKUSER) ? tmpData : result;
}

int HostSysCalls::brk(void *end_data_segment)
{
	//Parameters
	register long syscall asm("%rax") = 12;
	register long end asm("%rdi") = (long) end_data_segment;

	//Result
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (end) : "%rcx", "%r11");

	return result;
}

int HostSysCalls::unlink(const char *name)
{
    //Parameters
    register long syscall asm("%rax") = 87;
    register long _name asm("%rdi") = (long) name;

    //Result
    register long result asm("%rax");

    asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_name) : "%rcx", "%r11");

    return result;
}

unsigned long HostSysCalls::fork()
{
	/* Parameters */
	register long syscall asm("%rax") = 57;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall) : "%rcx", "%r11");

	return result;
}

int HostSysCalls::execve(const char *filename, char *const argv[], char *const envp[])
{
	/* Parameters */
	register long syscall asm("%rax") = 59;
	register long _filename asm("%rdi") = (long) filename;
	register long _argv asm("%rsi") = (long) argv;
	register long _envp asm("%rdx") = (long) envp;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_filename), "r" (_argv), "r" (_envp) : "%rcx", "%r11");

	return result;
}

int HostSysCalls::waitpid(int pid, int *status, int options)
{
	/* Parameters */
	register long syscall asm("%rax") = 61;
	register long _pid asm("%rdi") = pid;
	register long _status asm("%rsi") =  (long) status;
	register long _options asm("%rdx") = options;
    register long _rusage asm("%r10") = 0;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_pid), "r" (_status), "r" (_options), "r" (_rusage) : "%rcx", "%r11");

	*status = _status;

	return result;
}

int HostSysCalls::time(unsigned long *t)
{
	/* Parameters */
	register long syscall asm("%rax") = 13;
	register long tptr asm("%rbx") = (long) t;

	/* Result */
	register long res asm("%rax");

	asm volatile("int $0x80" : "=r" (res) : "r" (syscall), "r" (tptr) : "%rcx", "%r11");

	return res;
}

int HostSysCalls::clone(int (*fn)(void *), void *child_stack,  int flags, void *arg0, void *arg1)
{
    //We need to put args on the new stack
    
    /* Parameters */
    register long syscall asm("%rax") = 56;
    register long _flags asm("%rdi") = flags;
    register long _childStack asm("%rsi") = (long) child_stack;


    /* Result */
    register long res asm("%rax");

    asm volatile("syscall\n"
                 "cmp $0,%0\n"
                 "jne returnToParent\n"
                 "mov %5,%4\n"
                 "mov %6,%3\n"
                 "call *%2\n"
                 "mov $60, %0\n"
                 "syscall\n"
                 "returnToParent:\n"
                 : "=r" (res)
                 :"r" (syscall), "r" (fn), "r" (_childStack), "r" (_flags), "g" (arg0), "g" (arg1)
                 : "%rcx", "%r11", "cc");

    return res;
}

int HostSysCalls::newThread(int (*fn)(void *), void *arg0, void *arg1)
{
    return clone(fn, (void*) ((char *) malloc(8192) + 8192), CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, arg0, arg1);
}

struct mmap_arg_struct
{
    unsigned long addr;
    unsigned long len;
    unsigned long prot;
    unsigned long flags;
    unsigned long fd;
    unsigned long offset;
};

void *HostSysCalls::mmap(void *start, size_t length, int prot, int flags, int fd, long offset)
{
	/* Parameters */
	register long syscall asm("%rax") = 9;
	register long _start asm("%rdi") = (long) start;
	register long _length asm("%rsi") = (long) length;
	register long _prot asm("%rdx") = (long) prot;
	register long _flags asm("%r10") = (long) flags;
	register long _fd asm("%r8") = (long) fd;
	register long _offset asm("%r9") = (long) offset;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_start), "r" (_length), "r" (_prot), "r" (_flags), "r" (_fd), "r" (_offset) : "%rcx", "%r11");

	return (void *) result;
}

int HostSysCalls::ioctl(int d, int request, void *argp)
{
    /* Parameters */
    register long syscall asm("%rax") = 16;
    register long _d asm("%rdi") = d;
    register long _request asm("%rsi") =  (long) request;
    register long _argp asm("%rdx") = (long) argp;

    /* Result */
    register long result asm("%rax");

    asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_d), "r" (_request), "r" (_argp) : "%rcx", "%r11");

    return result;
}

int HostSysCalls::getpid()
{
    /* Parameters */
    register long syscall asm("%rax") = 39;

    /* Result */
    register long result asm("%rax");

    asm volatile("syscall" : "=r" (result) : "r" (syscall): "%rcx", "%r11");

    return result;
    
}
