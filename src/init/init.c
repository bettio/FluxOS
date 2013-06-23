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
#include <net/if.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <sys/fluxos.h>
#include <sys/socket.h>
#include <sys/wait.h>

void init_net(void);
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
    init_net();

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

void init_net(void)
{
    struct ifreq ifr;
    struct sockaddr_in sai;

    memset(&ifr, 0, sizeof(ifr));
    memset(&sai, 0, sizeof(sai));

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);

    sai.sin_family = PF_INET;
    sai.sin_port = 0;
    sai.sin_addr.s_addr = 0x5901A8C0;
    memcpy(&ifr.ifr_addr, &sai, sizeof(struct sockaddr));

    ioctl(sockfd, SIOCSIFADDR, &ifr);
    ioctl(sockfd, SIOCGIFFLAGS, &ifr);

    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    ioctl(sockfd, SIOCSIFFLAGS, &ifr);

    struct rtentry route;
    sai.sin_addr.s_addr = 0x0001A8C0;
    memcpy(&route.rt_dst, &sai, sizeof(struct sockaddr));
    sai.sin_addr.s_addr = 0x00000000;
    memcpy(&route.rt_gateway, &sai, sizeof(struct sockaddr));
    sai.sin_addr.s_addr = 0x00FFFFFF;
    memcpy(&route.rt_genmask, &sai, sizeof(struct sockaddr));
    route.rt_dev = "eth0";
    ioctl(sockfd, SIOCADDRT, &route);


    struct sockaddr_in6 sai6;
    memset(&sai6, 0, sizeof(sai6));
    sai6.sin6_family = PF_INET6;
    sai6.sin6_port = 0;
    sai6.sin6_addr.s6_addr[0] = 0x20;
    sai6.sin6_addr.s6_addr[1] = 0x01;
    sai6.sin6_addr.s6_addr[2] = 0x04;
    sai6.sin6_addr.s6_addr[3] = 0x70;
    sai6.sin6_addr.s6_addr[4] = 0x00;
    sai6.sin6_addr.s6_addr[5] = 0x6C;
    sai6.sin6_addr.s6_addr[6] = 0x00;
    sai6.sin6_addr.s6_addr[7] = 0x7E;
    sai6.sin6_addr.s6_addr[15] = 0x03;
    //memcpy(&ifr.ifr_addr, &sai, sizeof(struct sockaddr));

    //ioctl(sockfd, SIOCSIFADDR, &ifr);
    //ioctl(sockfd, SIOCGIFFLAGS, &ifr);

    //ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    //ioctl(sockfd, SIOCSIFFLAGS, &ifr);
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
