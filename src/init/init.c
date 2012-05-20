/***************************************************************************
 *   Copyright 2011 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: init.c                                                          *
 ***************************************************************************/

#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/dirent.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/fluxos.h>
#include <sys/wait.h>

void init_sethostname(void);
void init_setdomainname(void);

#define MINIMAL

int main(int argc, char *argv[])
{
    printf("\n\nINIT 0.1 - Starting FluxOS...\n\n");

#ifndef MINIMAL
    init_sethostname();
    init_setdomainname();
#endif
    
    while (1){
        printf("\n\nStarting the shell...\n\n");

        int status;
        int pid = fork();
        if (!pid){
            char *a[] = {"/bin/fluxsh", "", 0};
            execve("/bin/fluxsh", a, 0);
        }

        waitpid(pid, &status, 0);
    }

    return 0;
}

#ifndef MINIMAL
void init_sethostname(void)
{
    printf("Setting host name...");
    
    char *hname = malloc(256);
    read(open("/etc/hostname", 0, 0), hname, 256);
    sethostname(hname, 0);
    struct utsname unm;
    uname(&unm);
    printf("\t[  %s  ]\n", unm.nodename);
}

void init_setdomainname(void)
{
    printf("Setting domain name...");

    char *dname = malloc(256);
    read(open("/etc/domainname", 0, 0), dname, 256);
    setdomainname(dname, 0);
    struct utsname unm;
    uname(&unm);
    printf("\t[  %s  ]\n", unm.domainname);
}
#endif
