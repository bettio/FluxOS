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
 *   Name: syscalls.c                                                      *
 *   Date: 14/11/2015                                                      *
 ***************************************************************************/

#include <string.h>
#include <unistd.h>
#include <sys/dirent.h>
#include <sys/utsname.h>
#include <sys/socket.h>

#include "../../utils.h"

extern int errno;

#define __NR_SYSCALLS   4000
#define __NR_EXIT	4001
#define __NR_FORK	4002
#define __NR_READ	4003
#define __NR_WRITE	4004
#define __NR_OPEN	4005
#define __NR_CLOSE	4006
#define __NR_WAITPID    4007
#define __NR_CREAT	4008
#define __NR_LINK       4009
#define __NR_UNLINK	4010
#define __NR_EXECVE	4011
#define __NR_CHDIR	4012
#define __NR_TIME	4013
#define __NR_MKNOD      4014
#define __NR_CHMOD      4015
#define __NR_LCHOWN     4016
#define __NR_BREAK      4017
/* unused */
#define __NR_LSEEK      4019
#define __NR_GETPID     4020
#define __NR_MOUNT      4021
#define __NR_UMOUNT     4022
#define __NR_SETUID     4023
#define __NR_GETUID	4024
#define __NR_RENAME	2038
#define __NR_MKDIR	4039
#define __NR_RMDIR	4040
#define __NR_BRK        4045
#define __NR_GETGID     4047
#define __NR_GETEUID    4049
#define __NR_GETEGID    4050
#define __NR_UMOUNT2    4052
#define __NR_IOCTL	4054
#define __NR_FCNTL      4055
#define __NR_SETGID     4046
#define __NR_GETPPID    4064
#define __NR_SETSID     4066
#define __NR_SETHOSTNAME 4074
#define __NR_SYMLINK    4083
#define __NR_READLINK	4085
#define __NR_REBOOT	4088
#define __NR_MMAP       4090
#define __NR_MUNMAP     4091
#define __NR_TRUNCATE   4092
#define __NR_FTRUNCATE  4093
#define __NR_FCHMOD	4094
#define __NR_FCHOWN     4095
#define __NR_LSTAT	4107
#define __NR_FSTAT	4108
#define __NR_FSYNC      4118
#define __NR_SETDOMAINNAME 4121
#define __NR_MPROTECT   4125
#define __NR_UNAME      4122
#define __NR_GETDENTS	4141
#define __NR_MSYNC      4144
#define __NR_GETSID     4151
#define __NR_FDATASYNC  4152
#define __NR_MLOCK      4154
#define __NR_MUNLOCK    4155
#define __NR_MLOCKALL   4156
#define __NR_MUNLOCKALL 4157
#define __NR_MREMAP     4167
#define __NR_SOCKET     4183
#define __NR_SETRESUID  4185
#define __NR_SETRESGID  4190
#define __NR_CHOWN	4202
#define __NR_GETCWD	4203

#define SYSCALL_0(name, num, retT) \
    retT name() \
    { \
        register long syscall asm("$2") = num; \
        \
        register long result asm("$2"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall)); \
        \
        if (((int) result) >= 0){ \
            return result; \
        }else{ \
            errno = -result; \
            return (retT) -1; \
        } \
    }

#define SYSCALL_1(name, num, retT, arg0_t) \
    retT name(arg0_t arg0) \
    { \
        register long syscall asm("$2") = num; \
        register long _arg0 asm("$4") = (long) arg0; \
        \
        register long result asm("$2"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0)); \
        \
        if (((int) result) >= 0){ \
            return (retT) result; \
        }else{ \
            errno = -result; \
            return (retT) -1; \
        } \
    }

#define SYSCALL_2(name, num, retT, arg0_t, arg1_t) \
    retT name(arg0_t arg0, arg1_t arg1) \
    { \
        register long syscall asm("$2") = num; \
        register long _arg0 asm("$4") = (long) arg0; \
        register long _arg1 asm("$5") = (long) arg1; \
        \
        register long result asm("$2"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1)); \
        \
        if (((int) result) >= 0){ \
            return (retT) result; \
        }else{ \
            errno = -result; \
            return (retT) -1; \
        } \
    }

#define SYSCALL_3(name, num, retT, arg0_t, arg1_t, arg2_t) \
    retT name(arg0_t arg0, arg1_t arg1, arg2_t arg2) \
    { \
        register long syscall asm("$2") = num; \
        register long _arg0 asm("$4") = (long) arg0; \
        register long _arg1 asm("$5") = (long) arg1; \
        register long _arg2 asm("$6") = (long) arg2; \
        \
        register long result asm("$2"); \
        \
        asm volatile("syscall" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2)); \
        \
        if (((int) result) >= 0){ \
            return (retT) result; \
        }else{ \
            errno = -result; \
            return (retT) -1; \
        } \
    }

#define SYSCALL_4(name, num, retT, arg0_t, arg1_t, arg2_t, arg3_t) \
    retT name(arg0_t arg0, arg1_t arg1, arg2_t arg2, arg3_t arg3) \
{ \
    register long syscall asm("$2") = num; \
    register long _arg0 asm("$4") = (long) arg0; \
    register long _arg1 asm("$5") = (long) arg1; \
    register long _arg2 asm("$6") = (long) arg2; \
    \
    register long result asm("$2"); \
    \
    asm volatile( \
                 "addi $29, $29, -4\n" \
                 "sw %5, 0($29)\n" \
                 "sw %6, 4($29)\n" \
                 "syscall\n" \
                 "addi $29, $29, 4\n" \
                  : "=r" (result) \
                  : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2), "r" (arg3)); \
    if (((int) result) >= 0) { \
        return (retT) result; \
    } else { \
        errno = -result; \
        return (retT) -1; \
    } \
}

#define SYSCALL_5(name, num, retT, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t) \
    retT name(arg0_t arg0, arg1_t arg1, arg2_t arg2, arg3_t arg3, arg4_t arg4) \
{ \
    register long syscall asm("$2") = num; \
    register long _arg0 asm("$4") = (long) arg0; \
    register long _arg1 asm("$5") = (long) arg1; \
    register long _arg2 asm("$6") = (long) arg2; \
    \
    register long result asm("$2"); \
    \
    asm volatile( \
                 "addi $29, $29, -8\n" \
                 "sw %5, 0($29)\n" \
                 "sw %6, 4($29)\n" \
                 "syscall\n" \
                 "addi $29, $29, 8\n" \
                  : "=r" (result) \
\                  : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2), "r" (arg3), "r" (arg4)); \
    if (((int) result) >= 0) { \
        return (retT) result; \
    } else { \
        errno = -result; \
        return (retT) -1; \
    } \
}

#define SYSCALL_6(name, num, retT, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t) \
    retT name(arg0_t arg0, arg1_t arg1, arg2_t arg2, arg3_t arg3, arg4_t arg4, arg5_t arg5) \
{ \
    register long syscall asm("$2") = num; \
    register long _arg0 asm("$4") = (long) arg0; \
    register long _arg1 asm("$5") = (long) arg1; \
    register long _arg2 asm("$6") = (long) arg2; \
    \
    register long result asm("$2"); \
    \
    asm volatile( \
                 "addi $29, $29, -12\n" \
                 "sw %5, 0($29)\n" \
                 "sw %6, 4($29)\n" \
                 "sw %7, 8($29)\n" \
                 "syscall\n" \
                 "addi $29, $29, 12\n" \
                  : "=r" (result) \
                  : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2), "r" (arg3), "r" (arg4), "r" (arg5)); \
    if (((int) result) >= 0) { \
        return (retT) result; \
    } else { \
        errno = -result; \
        return (retT) -1; \
    } \
}

void _exit(int status)
{
    register long syscall asm("$2") = __NR_EXIT;
    register long _arg0 asm("$4") = (long) status;

    register long result asm("$2");

    asm volatile("syscall"
                 : "=r" (result)
                 : "r" (syscall), "r" (_arg0));
}

SYSCALL_0(fork, __NR_FORK, pid_t)
SYSCALL_3(read, __NR_READ, ssize_t, int, void *, size_t) //warn
SYSCALL_3(write, __NR_WRITE, int, int, const char*, size_t) //warn
SYSCALL_3(open, __NR_OPEN, int, const char *, int, mode_t)
SYSCALL_1(close, __NR_CLOSE, int, int)
SYSCALL_3(waitpid, __NR_WAITPID, int, pid_t, int *, int) //wait4
SYSCALL_3(creat, __NR_CREAT, int, const char *, int, mode_t)
SYSCALL_1(unlink, __NR_UNLINK, int, const char *)
SYSCALL_3(execve, __NR_EXECVE, int, const char *, char *const *, char *const *)
SYSCALL_1(chdir, __NR_CHDIR, int, const char *)
SYSCALL_1(time, __NR_TIME, time_t, time_t *)
SYSCALL_0(getuid, __NR_GETUID, uid_t)
SYSCALL_2(mkdir, __NR_MKDIR, int, const char *, mode_t)
SYSCALL_1(rmdir, __NR_RMDIR, int, const char *)
SYSCALL_1(brk, __NR_BRK, int, void *)
SYSCALL_2(sethostname, __NR_SETHOSTNAME, int, const char *, size_t)
SYSCALL_3(readlink, __NR_READLINK, ssize_t, const char *, char *, size_t)
SYSCALL_2(lstat, __NR_LSTAT, int, const char *, struct stat *)
SYSCALL_2(fstat, __NR_FSTAT, int, int, struct stat *)
SYSCALL_2(setdomainname, __NR_SETDOMAINNAME, int, const char *, size_t)
SYSCALL_3(mprotect, __NR_MPROTECT, int, void *, size_t, int)
SYSCALL_1(uname, __NR_UNAME, int, struct utsname *)
SYSCALL_3(getdents, __NR_GETDENTS, int, unsigned int, struct dirent *, unsigned int)
SYSCALL_3(msync, __NR_MSYNC, int, void *, size_t, int)
SYSCALL_2(mlock, __NR_MLOCK, int, const void *, size_t)
SYSCALL_2(munlock, __NR_MUNLOCK, int, const void *, size_t)
SYSCALL_1(mlockall, __NR_MLOCKALL, int, int)
SYSCALL_0(munlockall,  __NR_MUNLOCKALL, int)
SYSCALL_2(rename, __NR_RENAME, int, const char *, const char *)
//SYSCALL_4(reboot, __NR_REBOOT, int, int, int, int, void *)
SYSCALL_2(chmod, __NR_CHMOD, int, const char *, mode_t)
SYSCALL_2(munmap, __NR_MUNMAP, int, void *, size_t)
SYSCALL_2(fchmod, __NR_FCHMOD, int, int, mode_t)
SYSCALL_3(chown, __NR_CHOWN, int, const char *, uid_t, gid_t)
SYSCALL_3(fchown, __NR_FCHOWN, int, int, uid_t, gid_t)
SYSCALL_3(lchown, __NR_LCHOWN, int, const char *, uid_t, gid_t)
SYSCALL_2(link, __NR_LINK, int, const char *, const char *)
SYSCALL_2(symlink, __NR_SYMLINK, int, const char *, const char *)
SYSCALL_3(mknod, __NR_MKNOD, int, const char *, mode_t, dev_t)
//SYSCALL_5(mount, 165, int, const char *, const char *, const char *, unsigned long, const void *)
SYSCALL_2(umount2, __NR_UMOUNT2, int, const char *, int)
SYSCALL_3(socket, __NR_SOCKET, int, int, int, int);
SYSCALL_3(ioctl, __NR_IOCTL, int, int, int, int);
SYSCALL_2(getcwd, __NR_GETCWD, char *, char *, size_t); //TODO: custom impl?
SYSCALL_6(mmap, __NR_MMAP, void *, void *, size_t, int, int, int, off_t)

/* TODO IMPLEMENT ME */
int mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
    while(1);
    return 0;
}

/* TODO IMPLEMENT ME */
int reboot(int magic, int magic2, int cmd, void *arg)
{
    while(1);
    return 0;
}
