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
 *   Name: system.cpp                                                      *
 ***************************************************************************/

#include <core/system.h>

#include <task/task.h>
#include <task/scheduler.h>
#include <filesystem/errors.h>
#include <defs.h>
#include <cstring.h>

char *HostName = "flux_host";
char *DomainName = "flux_domain";

int SetHostName(const char *name, size_t len)
{
    if (Scheduler::currentThread()->parentProcess->uid == 0){
        HostName = strndup(name, MAX(len,256));
        return 0;

    }else{
        return -EPERM;
    }
}

int SetDomainName(const char *name, size_t len)
{
    if (Scheduler::currentThread()->parentProcess->uid == 0){
	    DomainName = strndup(name, MAX(len,256));
        return 0;

    }else{
        return -EPERM;
    }
}

int Uname(utsname *buf)
{
	#ifndef FAKE_UNAME
		strcpy(buf->sysname, "FluxOS");
		strncpy(buf->nodename, HostName, 65);
		strcpy(buf->release, "0.1");
		strcpy(buf->version, __DATE__);
		strncpy(buf->domainname, DomainName, 65);
	#else
		strcpy(buf->sysname, "Linux");
		strncpy(buf->nodename, "fakelinux", 65);
		strcpy(buf->release, "2.6.19-gentoo-r6");
		strcpy(buf->version, "#1 SMP PREEMPT Tue Feb 6 19:10:56 CET 2007");
		strcpy(buf->machine, "i686");
		strncpy(buf->domainname, "", 65);
	#endif

	return 0;
}
