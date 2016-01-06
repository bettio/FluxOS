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
 *   Name: hostuapi.cpp                                                    *
 ***************************************************************************/

#include <uapi/hostuapi.h>

#include <arch.h>
#ifndef ARCH_IA32
#include <uapi/syscallsnr.h>
#endif
#include <core/syscallsmanager.h>
#include <task/task.h>
#include <task/scheduler.h>
#include <errors.h>
#include <defs.h>
#include <cstring.h>

char *hostName;
char *domainName;

void HostUAPI::init()
{
#ifndef ARCH_IA32
    SyscallsManager::registerSyscall(__NR_SETHOSTNAME, (void *) sethostname);
    SyscallsManager::registerSyscall(__NR_SETDOMAINNAME, (void *) setdomainname);
    SyscallsManager::registerSyscall(__NR_UNAME, (void *) uname);
#endif
}

int HostUAPI::sethostname(userptr const char *name, size_t len)
{
    if (Scheduler::currentThread()->parentProcess->uid == 0){
        int newLen = MIN(len, 256);
        char *newHostName = (char *) malloc(newLen + 1);
        int ret = memcpyFromUser(newHostName, name, newLen);
        if (UNLIKELY(ret < 0)) {
            return ret;
        }
        newHostName[newLen] = 0;

        char *previousName = hostName;
        hostName = newHostName;
        if (previousName) {
            free(previousName);
        }
        return ret;

    }else{
        return -EPERM;
    }
}

int HostUAPI::setdomainname(userptr const char *name, size_t len)
{
    if (Scheduler::currentThread()->parentProcess->uid == 0){
        int newLen = MIN(len, 256);
        char *newDomainName = (char *) malloc(newLen + 1);
        int ret = memcpyFromUser(newDomainName, name, newLen);
        if (UNLIKELY(ret < 0)) {
            return ret;
        }
        newDomainName[newLen] = 0;

        char *previousName = domainName;
        domainName = newDomainName;
        if (previousName) {
            free(previousName);
        }
        return ret;

    }else{
        return -EPERM;
    }
}

int HostUAPI::uname(userptr utsname *buf)
{
    utsname unameBuf;
    memset(&unameBuf, 0, sizeof(struct utsname));
    #ifndef FAKE_UNAME
        strcpy(unameBuf.sysname, "FluxOS");
        if (hostName) {
            strncpy(unameBuf.nodename, hostName, 65);
        } else {
            strncpy(unameBuf.nodename, "", 65);
        }
        strcpy(unameBuf.release, "0.1");
        strcpy(unameBuf.version, __DATE__);
        strcpy(unameBuf.machine, UTSNAME_MACHINENAME);
        if (domainName) {
            strncpy(unameBuf.domainname, domainName, 65);
        } else {
            strncpy(unameBuf.domainname, "", 65);
        }
    #else
        strcpy(unameBuf.sysname, "Linux");
        strncpy(unameBuf.nodename, "fakelinux", 65);
        strcpy(unameBuf.release, "2.6.19-gentoo-r6");
        strcpy(unameBuf.version, "#1 SMP PREEMPT Tue Feb 6 19:10:56 CET 2007");
        strcpy(unameBuf.machine, "i686");
        strncpy(unameBuf.domainname, "", 65);
    #endif

    memcpyToUser(buf, &unameBuf, sizeof(struct utsname));

    return 0;
}
