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
 *   Name: hostsyscalls.h                                                  *
 *   Date: 06/08/2006                                                      *
 ***************************************************************************/

#include <kdef.h>
#include <cstring.h>

#include "hostsyscallsdefs.h"

class HostSysCalls
{
	public:
        static int munmap(void *addr, size_t length);
        static int close(int fd);
		static int open(const char *pathname, int flags, int mode = 0);
        static int creat(const char *pathname, int mode);
		static int write(int fd, const char *buf, size_t count);
		static int read(int fd, void *buf, size_t count);
		static int lseek(int fildes, uint64_t offset, int whence);
		static long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
		static int brk(void *end_data_segment);
        static int unlink(const char *name);
		static unsigned long fork();
		static int execve(const char *filename, char *const argv[], char *const envp[]);
		static int waitpid(int pid, int *status, int options);
		static int time(unsigned long *t);
		static int clone(int (*fn)(void *), void *child_stack,  int flags, void *arg0 = 0, void *arg1 = 0);
        static int newThread(int (*fn)(void *), void *arg0 = 0, void *arg1 = 0);
		static void *mmap(void *start, size_t length, int prot, int flags, int fd, long offset);
		static int ioctl(int d, int request, void *argp);
		static int getpid();
};
