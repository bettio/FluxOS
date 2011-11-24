/***************************************************************************
 *   Copyright 2006,2010 by Davide Bettio <davide.bettio@kdemail.net>      *
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
        register long syscall asm("%rax") = num; \
        register long _arg0 asm("%rdi") = (long) arg0; \
        register long _arg1 asm("%rsi") = (long) arg1; \
        register long _arg2 asm("%rdx") = (long) arg2; \
        \
        register long result asm("%rax"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2) \
                     : "%rcx", "%r11"); \
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
        register long syscall asm("%rax") = num; \
        register long _arg0 asm("%rdi") = (long) arg0; \
        register long _arg1 asm("%rsi") = (long) arg1; \
        register long _arg2 asm("%rdx") = (long) arg2; \
        register long _arg3 asm("%r10") = (long) arg3; \
        register long _arg4 asm("%r8") = (long) arg4; \
        \
        register long result asm("%rax"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2), "r" (_arg3), "r" (_arg4) \
                     : "%rcx", "%r11"); \
        \
        if (((int) result) >= 0){ \
            return result; \
        }else{ \
            errno = -result; \
            return -1; \
        } \
    }

SYSCALL_2(rename, 82, int, const char *, const char *)
SYSCALL_2(chmod, 90, int, const char *, mode_t)
SYSCALL_2(fchmod, 91, int, int, mode_t)
SYSCALL_3(chown, 92, int, const char *, uid_t, gid_t)
SYSCALL_3(fchown, 93, int, int, uid_t, gid_t)
SYSCALL_3(lchown, 94, int, const char *, uid_t, gid_t)
SYSCALL_2(link, 86, int, const char *, const char *)
SYSCALL_2(symlink, 88, int, const char *, const char *)
SYSCALL_3(mknod, 133, int, const char *, mode_t, dev_t)
SYSCALL_5(mount, 165, int, const char *, const char *, const char *, unsigned long, const void *)
SYSCALL_2(umount2, 166, int, const char *, int)

void _exit(int status)
{
	/* Parameters */
	register long syscall asm("%rax") = 60;
	register long _status asm("%rbx") = status;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall"  : "=r" (result) : "r" (syscall), "r" (_status));
}

pid_t fork(void)
{
	/* Parameters */
	register long syscall asm("%rax") = 57;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

pid_t CreateProcess(char *name, char *params)
{
	#if 1
	/* Parameters */
	register long syscall asm("%rax") = 220;
	register long _name asm("%rdi") = (long) name;
	register long _params asm("%rsi") = (long) params;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_name), "r" (_params));

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
	register long syscall asm("%rax") = 0;
	register long _fd asm("%rdi") = fd;
	register long _buf asm("%rsi") = (long) buf;
	register long _count asm("%rdx") = count;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_fd), "r" (_buf), "r" (_count));

	RETURN(result);
}

int write(int fd, const char *buf, size_t count)
{
	/* Parameters */
	register long syscall asm("%rax") = 1;
	register long _fd asm("%rdi") = fd;
	register long _buf asm("%rsi") = (long) buf;
	register long _count asm("%rdx") = count;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_fd), "r" (_buf), "r" (_count));

	RETURN(result);
}



int open(const char *pathname, int flags, ...)
{
	/* Parameters */
	register long syscall asm("%rax") = 2;
	register long _pathname asm("%rbx") = (long) pathname;
	register long _flags asm("%rcx") = flags;
	register long _mode asm("%rdx") = 0/*mode*/;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_pathname), "r" (_flags), "r" (_mode));

	RETURN(result);
}

int creat(const char *pathname, mode_t mode)
{
    /* Parameters */
    register long syscall asm("%rax") = 85;
    register long _pathname asm("%rdi") = (long) pathname;
    register long _mode asm("%rsi") = mode;

    /* Result */
    register long result asm("%rax");

    asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_pathname), "r" (_mode));

    RETURN(result);
}

int close(int fd)
{
	/* Parameters */
	register long syscall asm("%rax") = 3;
	register long _fd asm("%rbx") = (long) fd;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_fd));

	RETURN(result);
}

int waitpid(int pid, int *status, int options)
{
	/* Parameters */
	register long syscall asm("%rax") = 61;
	register long _pid asm("%rdi") = pid;
	register long _status asm("%rsi") =  (long) status;
	register long _options asm("%rdx") = options;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_pid), "r" (_status), "r" (_options));

	*status = _status;

	RETURN(result);
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
	/* Parameters */
	register long syscall asm("%rax") = 59;
	register long _filename asm("%rbx") = (long) filename;
	register long _argv asm("%rcx") = (long) argv;
	register long _envp asm("%rdx") = (long) envp;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_filename), "r" (_argv), "r" (_envp));

	RETURN(result);
}

int lseek(int fildes, int offset, int whence)
{
	/* Parameters */
	register long syscall asm("%rax") = 8;
	register long _fildes asm("%rbx") = fildes;
	register long _offset asm("%rcx") = (long) offset;
	register long _whence asm("%rdx") = whence;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_fildes), "r" (_offset), "r" (_whence));

	RETURN(result);
}

pid_t getpid()
{
	/* Parameters */
	register long syscall asm("%rax") = 39;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

int setuid(uid_t uid)
{
	/* Parameters */
	register long syscall asm("%rax") = 105;
	register long _uid asm("%rbx") = uid;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_uid));

	RETURN(result);
}

uid_t getuid()
{
	/* Parameters */
	register long syscall asm("%rax") = 102;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

int brk(void *end_data_segment)
{
	/* Parameters */
	register long syscall asm("%rax") = 12;
	register long _end_data_segment asm("%rbx") = (long) end_data_segment;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_end_data_segment));

	RETURN(result);
}


int setgid(gid_t gid)
{
	/* Parameters */
	register long syscall asm("%rax") = 106;
	register long _gid asm("%rbx") = gid;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_gid));

	RETURN(result);
}


gid_t getgid(void)
{
	/* Parameters */
	register long syscall asm("%rax") = 104;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

pid_t getppid(void)
{
	/* Parameters */
	register long syscall asm("%rax") = 110;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall));

	RETURN(result);
}

ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{
	/* Parameters */
	register long syscall asm("%rax") = 89;
	register long _path asm("%rbx") = (long) path;
	register long _buf asm("%rcx") = (long) buf;
	register long _bufsiz asm("%rdx") = bufsiz;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_path), "r" (_buf), "r" (_bufsiz));

	RETURN(result);
}

//TODO: It may be buggy
char *getcwd(char *buf, size_t size)
{
	/* Parameters */
	register long syscall asm("%rax") = 79;
	register long buffer asm("%rbx") = (long) buf;
	register long bufsize asm("%rcx") = (long) size;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (buffer), "r" (bufsize));

	return (char *) result;
}

int sethostname(const char *name, size_t len)
{
	/* Parameters */
	register long syscall asm("%rax") = 170;
	register long nm asm("%rbx") = (long) name;
	register long ln asm("%rcx") = (long) len;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (nm), "r" (ln));

	RETURN(result);
}

int setdomainname(const char *name, size_t len)
{
	/* Parameters */
	register long syscall asm("%rax") = 171;
	register long nm asm("%rbx") = (long) name;
	register long ln asm("%rcx") = (long) len;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (nm), "r" (ln));

	RETURN(result);
}


int chdir(const char *path)
{
	/* Parameters */
	register long syscall asm("%rax") = 80;
	register long dirpath asm("%rbx") = (long) path;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (dirpath));

	RETURN(result);
}

int uname(struct utsname *buf)
{
	/* Parameters */
	register long syscall asm("%rax") = 63;
	register long utsn asm("%rdi") = (long) buf;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (utsn));

	RETURN(result);
}

time_t time(time_t *t)
{
	/* Parameters */
	register long syscall asm("%rax") = 13;
	register long tptr asm("%rbx") = (long) t;

	/* Result */
	register long result asm("%rax");

	asm volatile("int $0x80" : "=r" (result) : "r" (syscall), "r" (tptr));

	RETURN(result);
}

int getdents(unsigned int fd, struct dirent *dirp, unsigned int count)
{
	/* Parameters */
	register long syscall asm("%rax") = 78;
	register long fdesc asm("%rbx") = fd;
	register long buffer asm("%rcx") = (long) dirp;
	register long size asm("%rdx") = count;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (fdesc), "r" (buffer), "r" (size));

	RETURN(result);
}

int stat(const char *path, struct stat *buf)
{
	/* Parameters */
	register long syscall asm("%rax") = 4;
	register long pt asm("%rdi") = (long) path;
	register long bf asm("%rsi") = (long) buf;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (pt), "r" (bf));

	RETURN(result);
}

int fstat(int filedes, struct stat *buf)
{
	/* Parameters */
	register long syscall asm("%rax") = 5;
	register long fd asm("%rdi") = (long) filedes;
	register long bf asm("%rsi") = (long) buf;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (fd), "r" (bf));

	RETURN(result);
}

int lstat(const char *path, struct stat *buf)
{
	/* Parameters */
	register long syscall asm("%rax") = 6;
	register long pt asm("%rdi") = (long) path;
	register long bf asm("%rsi") = (long) buf;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (pt), "r" (bf));

	RETURN(result);
}

int ioctl(int d, int request, /*...*/ long *addr)
{
	/* Parameters */
	register long syscall asm("%rax") = 16;
	register long _d asm("%rbx") = d;
	register long _request asm("%rcx") = (long) request;
	register long _addr asm("%rdx") = (long) addr;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (_d), "r" (_request), "r" (_addr));

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
	register long syscall asm("%rax") = 9;
	register long argAddr asm("%rbx") = &mmapArg;

	/* Result */
	register long result asm("%rax");

	asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (argAddr));

	return result;
}

int rmdir(const char *path)
{
    /* Parameters */
    register long syscall asm("%rax") = 84;
    register long dirpath asm("%rbx") = (long) path;

    /* Result */
    register long result asm("%rax");

    asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (dirpath));

    RETURN(result);
}

int unlink(const char *path)
{
    /* Parameters */
    register long syscall asm("%rax") = 87;
    register long dirpath asm("%rbx") = (long) path;

    /* Result */
    register long result asm("%rax");

    asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (dirpath));

    RETURN(result);
}

int mkdir(const char *path, mode_t mode)
{
    /* Parameters */
    register long syscall asm("%rax") = 83;
    register long pt asm("%rdi") = (long) path;
    register long bf asm("%rsi") = (long) mode;

    /* Result */
    register long result asm("%rax");

    asm volatile("syscall" : "=r" (result) : "r" (syscall), "r" (pt), "r" (bf));

    RETURN(result);
}
