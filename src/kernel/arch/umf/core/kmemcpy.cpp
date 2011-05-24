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
 *   Name: kmemcpy.cpp                                                     *
 *   Date: 06/08/2006                                                      *
 ***************************************************************************/

#include <arch/umf/core/hostsyscalls.h>

#include <cstdlib.h>
#include <cstring.h>

union registerstring
{
	long value;
	char str[sizeof(long)];
};

void MemCpyFromKernelToUser(void *dest, const void *src, size_t n, pid_t pid)
{
	long *tmpDest = (long *) dest;
	long *tmpSrc = (long *) src;
	long *loopEndAddr = (long *) (((unsigned long) src) + n - (n % sizeof(long)));

	while (tmpSrc < loopEndAddr){
		HostSysCalls::ptrace(PTRACE_POKEDATA, pid, (void *) tmpDest, (void *) *tmpSrc);

		tmpSrc++;
		tmpDest++;
	}

	int remainingBytes = n % sizeof(long);

	if (remainingBytes){
		long tmpLong = HostSysCalls::ptrace(PTRACE_PEEKDATA, pid, (void *) tmpDest, (void *) NULL);
		memcpy(&tmpLong, tmpSrc, remainingBytes);
		HostSysCalls::ptrace(PTRACE_POKEDATA, pid, (void *) tmpDest, (void *) *tmpSrc);
	}
}

void MemCpyFromUserToKernel(void *dest, const void *src, size_t n, pid_t pid)
{
	registerstring *tmpDest = (registerstring *) dest;
	long *tmpSrc = (long *) src;
	long *loopEndAddr = (long *) (((unsigned long) src) + n - (n % sizeof(long)));

	while (tmpSrc < loopEndAddr){
		tmpDest->value = HostSysCalls::ptrace(PTRACE_PEEKDATA, pid, (void *) tmpSrc, (void *) NULL);

		tmpSrc++;
		tmpDest++;
	}

	int remainingBytes = n % sizeof(long);

	if (remainingBytes){
		long tmpLong = HostSysCalls::ptrace(PTRACE_PEEKDATA, pid, (void *) tmpSrc, (void *) NULL);
		memcpy(tmpDest, &tmpLong, remainingBytes);
	}
}

void StrNCpyFromUserToKernel(char *dest, const char *src, size_t n, pid_t pid)
{
	long *tmpDest = (long *) dest;
	long *tmpSrc = (long *) src;
	long *loopEndAddr = (long *) (((unsigned long) src) + n - (n % sizeof(long)));

	while (tmpSrc < loopEndAddr){
		*tmpDest = HostSysCalls::ptrace(PTRACE_PEEKDATA, pid, (void *) tmpSrc, (void *) NULL);

		tmpSrc++;
		tmpDest++;
	}

	int remainingBytes = n % sizeof(long);

	if (remainingBytes){
		long tmpLong = HostSysCalls::ptrace(PTRACE_PEEKDATA, pid, (void *) tmpSrc, (void *) NULL);
		memcpy(tmpDest, &tmpLong, remainingBytes);
	}
}
