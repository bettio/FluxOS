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
 *   Name: syscalls.c                                                      *
 *   Date: 30/10/2006                                                      *
 ***************************************************************************/

#include <string.h>
#include <unistd.h>
#include <sys/dirent.h>
#include <sys/utsname.h>

#include "../../utils.h"

extern int errno;

#define SYSCALL_1(name, num, retT, arg0_t) \
    retT name(arg0_t arg0) \
    { \
        register long syscall asm("%eax") = num; \
        register long _arg0 asm("%ebx") = (long) arg0; \
        \
        register long result asm("%eax"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0)); \
        \
        if (((int) result) >= 0){ \
            return result; \
        }else{ \
            errno = -result; \
            return -1; \
        } \
    }

#define SYSCALL_2(name, num, retT, arg0_t, arg1_t) \
    retT name(arg0_t arg0, arg1_t arg1) \
    { \
        register long syscall asm("%eax") = num; \
        register long _arg0 asm("%ebx") = (long) arg0; \
        register long _arg1 asm("%ecx") = (long) arg1; \
        \
        register long result asm("%eax"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1)); \
        \
        if (((int) result) >= 0){ \
            return result; \
        }else{ \
            errno = -result; \
            return -1; \
        } \
    }

#define SYSCALL_3(name, num, retT, arg0_t, arg1_t, arg2_t) \
    retT name(arg0_t arg0, arg1_t arg1, arg2_t arg2) \
    { \
        register long syscall asm("%eax") = num; \
        register long _arg0 asm("%ebx") = (long) arg0; \
        register long _arg1 asm("%ecx") = (long) arg1; \
        register long _arg2 asm("%edx") = (long) arg2; \
        \
        register long result asm("%eax"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2)); \
        \
        if (((int) result) >= 0){ \
            return result; \
        }else{ \
            errno = -result; \
            return -1; \
        } \
    }

#define SYSCALL_5(name, num, retT, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t) \
    retT name(arg0_t arg0, arg1_t arg1, arg2_t arg2, arg3_t arg3, arg4_t arg4) \
    { \
        register long syscall asm("%eax") = num; \
        register long _arg0 asm("%ebx") = (long) arg0; \
        register long _arg1 asm("%ecx") = (long) arg1; \
        register long _arg2 asm("%edx") = (long) arg2; \
        register long _arg3 asm("%esi") = (long) arg3; \
        register long _arg4 asm("%edi") = (long) arg4; \
        \
        register long result asm("%eax"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2), "r" (_arg3), "r" (_arg4)); \
        \
        if (((int) result) >= 0){ \
            return result; \
        }else{ \
            errno = -result; \
            return -1; \
        } \
    }

SYSCALL_1(unlink, 10, int, const char *)
SYSCALL_1(rmdir, 40, int, const char *)
SYSCALL_2(rename, 38, int, const char *, const char *)
SYSCALL_2(mkdir, 39, int, const char *, mode_t)
SYSCALL_2(chmod, 15, int, const char *, mode_t)
SYSCALL_2(fchmod, 94, int, int, mode_t)
SYSCALL_3(chown, 182, int, const char *, uid_t, gid_t)
SYSCALL_3(fchown, 95, int, int, uid_t, gid_t)
SYSCALL_3(lchown, 16, int, const char *, uid_t, gid_t)
SYSCALL_2(link, 9, int, const char *, const char *)
SYSCALL_2(symlink, 83, int, const char *, const char *)
SYSCALL_3(mknod, 14, int, const char *, mode_t, dev_t)
SYSCALL_5(mount, 21, int, const char *, const char *, const char *, unsigned long, const void *)
SYSCALL_2(umount2, 52, int, const char *, int)
SYSCALL_2(creat, 8, int, const char *, mode_t)

void _exit(int status)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 1;
	register unsigned int _status asm("%ebx") = status;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80"  : "=r" (result) : "r" (syscall), "r" (_status));
}

pid_t fork(void)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 2;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

pid_t CreateProcess(char *name, char *params)
{
	#if 1
	/* Parameters */
	register unsigned int syscall asm("%eax") = 220;
	register unsigned int _name asm("%ebx") = (unsigned int) name;
	register unsigned int _params asm("%ecx") = (unsigned int) params;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_name), "r" (_params));

	RETURN(result);
	#endif

	#if 0
	pid_t pid = fork();

	char *params_[] = {"", 0};
	char *env[] = {"", 0};

	if (!pid) execve(name, params_, env);
	#endif
}

ssize_t read(int fd, void *buf, size_t count)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 3;
	register unsigned int _fd asm("%ebx") = fd;
	register unsigned int _buf asm("%ecx") = (unsigned int) buf;
	register unsigned int _count asm("%edx") = count;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_fd), "r" (_buf), "r" (_count));

	RETURN(result);
}

int write(int fd, const char *buf, size_t count)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 4;
	register unsigned int _fd asm("%ebx") = fd;
	register unsigned int _buf asm("%ecx") = (unsigned int) buf;
	register unsigned int _count asm("%edx") = count;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_fd), "r" (_buf), "r" (_count));

	RETURN(result);
}



int open(const char *pathname, int flags, ...)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 5;
	register unsigned int _pathname asm("%ebx") = (unsigned int) pathname;
	register unsigned int _flags asm("%ecx") = flags;
	register unsigned int _mode asm("%edx") = 0/*mode*/;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_pathname), "r" (_flags), "r" (_mode));

	RETURN(result);
}

int close(int fd)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 6;
	register unsigned int _fd asm("%ebx") = (unsigned int) fd;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_fd));

	RETURN(result);
}

int waitpid(int pid, int *status, int options)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 7;
	register unsigned int _pid asm("%ebx") = pid;
	register unsigned int _status asm("%ecx") =  (unsigned int) status;
	register unsigned int _options asm("%edx") = options;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_pid), "r" (_status), "r" (_options));

	*status = _status;

	RETURN(result);
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 11;
	register unsigned int _filename asm("%ebx") = (unsigned int) filename;
	register unsigned int _argv asm("%ecx") = (unsigned int) argv;
	register unsigned int _envp asm("%edx") = (unsigned int) envp;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_filename), "r" (_argv), "r" (_envp));

	RETURN(result);
}

int lseek(int fildes, int offset, int whence)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 19;
	register unsigned int _fildes asm("%ebx") = fildes;
	register unsigned int _offset asm("%ecx") = (unsigned int) offset;
	register unsigned int _whence asm("%edx") = whence;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_fildes), "r" (_offset), "r" (_whence));

	RETURN(result);
}

pid_t getpid()
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 20;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

int setuid(uid_t uid)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 23;
	register unsigned int _uid asm("%ebx") = uid;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_uid));

	RETURN(result);
}

uid_t getuid()
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 24;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

int brk(void *end_data_segment)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 45;
	register unsigned int _end_data_segment asm("%ebx") = (unsigned int) end_data_segment;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_end_data_segment));

	RETURN(result);
}


int setgid(gid_t gid)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 46;
	register unsigned int _gid asm("%ebx") = gid;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_gid));

	RETURN(result);
}


gid_t getgid(void)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 47;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

pid_t getppid(void)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 64;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 85;
	register unsigned int _path asm("%ebx") = (unsigned int) path;
	register unsigned int _buf asm("%ecx") = (unsigned int) buf;
	register unsigned int _bufsiz asm("%edx") = bufsiz;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_path), "r" (_buf), "r" (_bufsiz));

	RETURN(result);
}

//TODO: It might be buggy
char *getcwd(char *buf, size_t size)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 183;
	register unsigned int buffer asm("%ebx") = (unsigned int) buf;
	register unsigned int bufsize asm("%ecx") = (unsigned int) size;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (buffer), "r" (bufsize));

	return (char *) result;
}

int sethostname(const char *name, size_t len)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 74;
	register unsigned int nm asm("%ebx") = (unsigned int) name;
	register unsigned int ln asm("%ecx") = (unsigned int) len;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (nm), "r" (ln));

	RETURN(result);
}

int setdomainname(const char *name, size_t len)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 121;
	register unsigned int nm asm("%ebx") = (unsigned int) name;
	register unsigned int ln asm("%ecx") = (unsigned int) len;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (nm), "r" (ln));

	RETURN(result);
}


int chdir(const char *path)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 12;
	register unsigned int dirpath asm("%ebx") = (unsigned int) path;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (dirpath));

	RETURN(result);
}

int uname(struct utsname *buf)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 122;
	register unsigned int utsn asm("%ebx") = (unsigned int) buf;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (utsn));

	RETURN(result);
}

time_t time(time_t *t)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 13;
	register unsigned int tptr asm("%ebx") = (unsigned int) t;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (tptr));

	RETURN(result);
}

int getdents(unsigned int fd, struct dirent *dirp, unsigned int count)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 141;
	register unsigned int fdesc asm("%ebx") = fd;
	register unsigned int buffer asm("%ecx") = (unsigned int) dirp;
	register unsigned int size asm("%edx") = count;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (fdesc), "r" (buffer), "r" (size));

	RETURN(result);
}

int stat(const char *path, struct stat *buf)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 18;
	register unsigned int pt asm("%ebx") = (unsigned int) path;
	register unsigned int bf asm("%ecx") = (unsigned int) buf;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (pt), "r" (bf));

	RETURN(result);
}

int fstat(int filedes, struct stat *buf)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 28;
	register unsigned int fd asm("%ebx") = (unsigned int) filedes;
	register unsigned int bf asm("%ecx") = (unsigned int) buf;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (fd), "r" (bf));

	RETURN(result);
}

int lstat(const char *path, struct stat *buf)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 107;
	register unsigned int pt asm("%ebx") = (unsigned int) path;
	register unsigned int bf asm("%ecx") = (unsigned int) buf;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (pt), "r" (bf));

	RETURN(result);
}

int ioctl(int d, int request, /*...*/ long *addr)
{
	/* Parameters */
	register unsigned int syscall asm("%eax") = 54;
	register unsigned int _d asm("%ebx") = d;
	register unsigned int _request asm("%ecx") = (unsigned int) request;
	register unsigned int _addr asm("%edx") = (unsigned int) addr;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (_d), "r" (_request), "r" (_addr));

	RETURN(result);
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

void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
	struct mmap_arg_struct mmapArg;
	mmapArg.addr = (unsigned long) start;
	mmapArg.len = (unsigned long) length;
	mmapArg.prot = (unsigned long) prot;
	mmapArg.flags = (unsigned long) flags;
	mmapArg.fd = (unsigned long) fd;
	mmapArg.offset = (unsigned long) offset;

	/* Parameters */
	register unsigned int syscall asm("%eax") = 90;
	register unsigned int argAddr asm("%ebx") = &mmapArg;

	/* Result */
	register unsigned int result asm("%eax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (argAddr));

	return result;
}
