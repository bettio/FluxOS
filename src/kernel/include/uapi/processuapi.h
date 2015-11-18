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
 *   Name: processuapi.h                                                   *
 *   Date: 17/11/2015                                                      *
 ***************************************************************************/

#ifndef _UAPI_PROCESSUAPI_H_
#define _UAPI_PROCESSUPAI_H_

#include <kdef.h>

#define pid_t unsigned long
#define uid_t unsigned long
#define gid_t unsigned long

class ProcessUAPI
{
    public:
        static void init();

        static pid_t getpid();
        static pid_t getppid();
        static pid_t getsid(pid_t pid);
        static uid_t getuid();
        static uid_t geteuid();
        static gid_t getgid();
        static gid_t getegid();

        static pid_t setsid();

        static int setuid(uid_t uid);
        static int seteuid(uid_t euid);
        static int setresuid(uid_t ruid, uid_t euid, uid_t suid);
        static int setgid(gid_t gid);
        static int setegid(gid_t egid);
        static int setresgid(gid_t rgid, gid_t egid, gid_t sgid);
};

#undef pid_t
#undef uid_t
#undef gid_t

#endif
