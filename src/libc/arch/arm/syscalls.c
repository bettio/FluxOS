/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 09/2010                                                         *
 ***************************************************************************/

#include <string.h>
#include <unistd.h>
#include <sys/dirent.h>
#include <sys/utsname.h>

#include "../../utils.h"

extern int errno;

#define SYSCALL_0(name, num, retT) \
    retT name(void) \
    { \
        register long syscall asm("r7") = num; \
        \
        register long result asm("r0"); \
        \
        asm volatile("svc 0x0" \
                     : "=r" (result) \
                     : "r" (syscall)); \
        \
        if (((int) result) >= 0){ \
            return result; \
        }else{ \
            errno = -result; \
            return -1; \
        } \
    }

#define SYSCALL_1(name, num, retT, arg0_t) \
    retT name(arg0_t arg0) \
    { \
        register long syscall asm("r7") = num; \
        register long _arg0 asm("r0") = (long) arg0; \
        \
        register long result asm("r0"); \
        \
        asm volatile("svc 0x0" \
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
        register long syscall asm("r7") = num; \
        register long _arg0 asm("r0") = (long) arg0; \
        register long _arg1 asm("r1") = (long) arg1; \
        \
        register long result asm("r0"); \
        \
        asm volatile("svc 0x0" \
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
        register long syscall asm("r7") = num; \
        register long _arg0 asm("r0") = (long) arg0; \
        register long _arg1 asm("r1") = (long) arg1; \
        register long _arg2 asm("r2") = (long) arg2; \
        \
        register long result asm("r0"); \
        \
        asm volatile("svc 0x0" \
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

#define SYSCALL_4(name, num, retT, arg0_t, arg1_t, arg2_t, arg3_t) \
    retT name(arg0_t arg0, arg1_t arg1, arg2_t arg2, arg3_t arg3) \
    { \
        register long syscall asm("r7") = num; \
        register long _arg0 asm("r0") = (long) arg0; \
        register long _arg1 asm("r1") = (long) arg1; \
        register long _arg2 asm("r2") = (long) arg2; \
        register long _arg3 asm("r3") = (long) arg3; \
        \
        register long result asm("r0"); \
        \
        asm volatile("svc 0x0" \
                     : "=r" (result) \
                     : "r" (syscall), "r" (_arg0), "r" (_arg1), "r" (_arg2), "r" (_arg3)); \
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
        register long syscall asm("r7") = num; \
        register long _arg0 asm("r0") = (long) arg0; \
        register long _arg1 asm("r1") = (long) arg1; \
        register long _arg2 asm("r2") = (long) arg2; \
        register long _arg3 asm("r3") = (long) arg3; \
        register long _arg4 asm("r4") = (long) arg4; \
        \
        register long result asm("r0"); \
        \
        asm volatile("svc 0x0" \
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

#define __NR_FORK 2
#define __NR_READ 3
#define __NR_WRITE 4
#define __NR_OPEN 5
#define __NR_CREAT 8
#define __NR_UNLINK 10
#define __NR_EXECVE 11
#define __NR_CHDIR 12
#define __NR_TIME 13
#define __NR_GETUID 24
#define __NR_MKDIR 39
#define __NR_RMDIR 40
#define __NR_READLINK 85
#define __NR_REBOOT 88
#define __NR_LSTAT 107
#define __NR_GETDENTS 141
#define __NR_GETCWD 183

SYSCALL_1(_exit, 1, void, int)
SYSCALL_0(fork, 2, pid_t)
SYSCALL_3(read, __NR_READ, ssize_t, int, void *, size_t) //warn
SYSCALL_3(write, __NR_WRITE, int, int, const char*, size_t) //warn
SYSCALL_3(open, __NR_OPEN, int, const char *, int, mode_t)
SYSCALL_3(creat, __NR_CREAT, int, const char *, int, mode_t)
SYSCALL_1(close, 6, int, int)
SYSCALL_1(unlink, __NR_UNLINK, int, const char *)
SYSCALL_3(execve, __NR_EXECVE, int, const char *, char *const *, char *const *)
SYSCALL_1(chdir, __NR_CHDIR, int, const char *)
SYSCALL_1(time, __NR_TIME, time_t, time_t *)
SYSCALL_0(getuid, __NR_GETUID, uid_t)
SYSCALL_2(mkdir, __NR_MKDIR, int, const char *, mode_t)
SYSCALL_1(rmdir, __NR_RMDIR, int, const char *)
SYSCALL_1(brk, 45, int, void *)
SYSCALL_2(sethostname, 74, int, const char *, size_t)
SYSCALL_3(readlink, __NR_READLINK, ssize_t, const char *, char *, size_t)
SYSCALL_2(lstat, __NR_LSTAT, int, const char *, struct stat *)
SYSCALL_3(waitpid, 114, int, pid_t, int *, int) //wait4
SYSCALL_2(setdomainname, 121, int, const char *, size_t)
SYSCALL_1(uname, 122, int, struct utsname *)
SYSCALL_2(CreateProcess, 220, int, const char *, const char *)
SYSCALL_3(getdents, __NR_GETDENTS, int, unsigned int, struct dirent *, unsigned int)



SYSCALL_2(rename, 82, int, const char *, const char *)
SYSCALL_4(reboot, __NR_REBOOT, int, int, int, int, void *)
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

//TODO: getcwd implementation
char *getcwd(char *buf, size_t size)
{
    //SYSCALL_2_BODY(getcwd, __NR_GETCWD, int, retVal, char *, size_t)
}

//TODO: please, move away this stuff
void __aeabi_unwind_cpp_pr0()
{

}
void raise()
{

}

