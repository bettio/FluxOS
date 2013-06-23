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
 *   Name: syscallsmanager.h                                               *
 *   Date: 29/09/2010                                                      *
 ***************************************************************************/

#ifndef _SYSCALLSMANAGER_H_
#define _SYSCALLSMANAGER_H_

#include <stdint.h>

//Socket calls
#define    SYS_SOCKET       1
#define    SYS_BIND         2
#define    SYS_CONNECT      3
#define    SYS_LISTEN       4
#define    SYS_ACCEPT       5
#define    SYS_GETSOCKNAME  6
#define    SYS_GETPEERNAME  7
#define    SYS_SOCKETPAIR   8
#define    SYS_SEND         9
#define    SYS_RECV         10
#define    SYS_SENDTO       11
#define    SYS_RECVFROM     12
#define    SYS_SHUTDOWN     13
#define    SYS_SETSOCKOPT   14
#define    SYS_GETSOCKOPT   15
#define    SYS_SENDMSG      16
#define    SYS_RECVMSG      17

class SyscallsManager
{
    public:
        static void init();
        static void registerSyscall(int num, uint32_t (*func)(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi));
        static void unregisterSyscall(int sysC);
        static uint32_t nullSysCallHandler(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
        
    private:
        static void registerDefaultSyscalls();
};

#endif
 
