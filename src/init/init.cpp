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
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <sys/dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <net/if.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <sys/fluxos.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define SERVICES_DIR "/usr/system/services/"
#define MOUNTS_DIR "/usr/system/mounts/"
#define NETWORK_DIR "/var/lib/network/"
#define NETWORK_ROUTES_DIR "/var/lib/network/routes/"
#define NETWORK_INTERFACES_DIR "/var/lib/network/interfaces/"

#include "libDataStore/bson.c"
#include "libDataStore/StoredObject.cpp"
#include "libDataStore/QHash"

void setHostName();
void setDomainName();
int configureNetwork();
int mountFilesystems();
int startServices(int argc, char *argv[], char *envp[]);
void processStatusChange(int pid, int status, char *envp[]);

QHash<int, const char *> *processes;

int main(int argc, char *argv[], char *envp[])
{
    printf("\n\ninit 0.1 - Starting FluxOS...\n\n");

    processes = new QHash<int, const char *>;

    mountFilesystems();
    configureNetwork();

    setHostName();
    setDomainName();

    startServices(argc, argv, envp);

    //TODO: event loop here
    while (1) {
        int status;
        int pid = waitpid(-1, &status, 0);
        processStatusChange(pid, status, envp);
    }
}

void processStatusChange(int pid, int status, char *envp[])
{
    const char *pExec = processes->value(pid);
    printf("init: restarting %s\n", pExec);

    int oldPid = pid;
    pid = fork();
    if (!pid){
        char *a[] = {(char *) pExec, 0};
        if (execve(pExec, a, envp)) {
            perror("init: cannot execute: ");
        }
    } else {
        processes->insert(pid, strdup(pExec));
        processes->remove(oldPid);
    }
}

int startServices(int argc, char *argv[], char *envp[])
{
    int dirFd = open(SERVICES_DIR, O_RDONLY | O_CLOEXEC);
    DIR *servicesDir = fdopendir(dirFd);

    if (!servicesDir) {
        printf("init: failed to open %s directory.\n", SERVICES_DIR);
        return -1;
    }
    struct dirent dirEntry;
    struct dirent *entry = NULL;

    while ((readdir_r(servicesDir, &dirEntry, &entry) == 0) && (entry != NULL)) {
        //TODO: use openat
        char buf[255];
        sprintf(buf, "%s%s", SERVICES_DIR, dirEntry.d_name);

        if (dirEntry.d_name[0] != '.') {
            StoredObject service = StoredObject::loadObject(buf);
            if (!service.isValid()) {
                printf("init: invalid service file: %s\n", buf);
                continue;
            }
            const char *serviceExecutable = service.value("exec", (const char *) NULL);

            printf("Starting %s...", dirEntry.d_name);

            if (!serviceExecutable) {
                printf("init: Cannot launch %s: exec property is not set.\n", buf);
                continue;
            }

            int pid = fork();
            if (!pid){
                char *a[] = {(char *) serviceExecutable, NULL};
                if (execve(serviceExecutable, a, envp)) {
                    perror("init: cannot execute: ");
                }
            } else {
                processes->insert(pid, strdup(serviceExecutable));
            }

            printf("\n");
        }
    }

    closedir(servicesDir);

    return EXIT_SUCCESS;
}

int mountFilesystems()
{
    DIR *mountsDir = opendir(MOUNTS_DIR);
    if (!mountsDir) {
        printf("init: failed to open %s directory.\n", MOUNTS_DIR);
        return -1;
    }
    struct dirent dirEntry;
    struct dirent *entry = NULL;

    while ((readdir_r(mountsDir, &dirEntry, &entry) == 0) && (entry != NULL)) {
        //TODO: use openat
        char buf[255];
        sprintf(buf, "%s%s", MOUNTS_DIR, dirEntry.d_name);

        if (dirEntry.d_name[0] != '.') {
            StoredObject mountObj = StoredObject::loadObject(buf);
            if (!mountObj.isValid()) {
                printf("init: invalid mount file: %s\n", buf);
                continue;
            }
            const char *mountWhere = mountObj.value("where", (const char *) NULL);
            const char *mountWhat = mountObj.value("what", (const char *) NULL);
            const char *mountType = mountObj.value("type", (const char *) NULL);

            if (!mountWhere || !mountWhat || !mountType) {
                printf("init: missing mount property in %s.\n", buf);
                continue;
            }

            if (mount(mountWhat, mountWhere, mountType, 0, NULL) < 0) {
                printf("init: failed to mount %s on %s", mountWhat, mountWhere);
                perror("");
            }
        }
    }
    closedir(mountsDir);
}

int configureNetworkInterfaces(int sockfd)
{
    printf("Configuring network interfaces... ");
    DIR *networkInterfacesDir = opendir(NETWORK_INTERFACES_DIR);
    if (!networkInterfacesDir) {
        printf("init: failed to open %s directory.\n", NETWORK_INTERFACES_DIR);
        return -1;
    }
    struct dirent dirEntry;
    struct dirent *entry = NULL;

    while ((readdir_r(networkInterfacesDir, &dirEntry, &entry) == 0) && (entry != NULL)) {
         //TODO: use openat
        char buf[255];
        sprintf(buf, "%s%s", NETWORK_INTERFACES_DIR, dirEntry.d_name);

        if (dirEntry.d_name[0] != '.') {
            StoredObject routeObj = StoredObject::loadObject(buf);
            if (!routeObj.isValid()) {
                printf("init: invalid network file: %s\n", buf);
                continue;
            }
            const char *networkInterface = routeObj.value("interface", (const char *) NULL);
            const char *ipv4Address = routeObj.value("ipv4Address", (const char *) NULL);
            const char *ipv6Address = routeObj.value("ipv6Address", (const char *) NULL);

            if (!networkInterface) {
                printf("init: cannot load %s: network interface has not been specified.\n", buf);
                continue;
            }

            struct ifreq ifr;
            struct sockaddr_in sai;

            memset(&ifr, 0, sizeof(ifr));
            memset(&sai, 0, sizeof(sai));

            strncpy(ifr.ifr_name, networkInterface, IFNAMSIZ);

            if (ipv4Address) {
                if (!inet_pton(0, ipv4Address, &sai.sin_addr.s_addr)) {
                    printf("init: %s invalid IPv4 interface address\n", buf);
                }
                sai.sin_family = PF_INET;
                sai.sin_port = 0;
                memcpy(&ifr.ifr_addr, &sai, sizeof(struct sockaddr));

                ioctl(sockfd, SIOCSIFADDR, &ifr);
                ioctl(sockfd, SIOCGIFFLAGS, &ifr);

                ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
                ioctl(sockfd, SIOCSIFFLAGS, &ifr);
            }
            printf(" [ %s ] ", networkInterface);
       }
    }

    closedir(networkInterfacesDir);
    printf("\n");
}

#if 0
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
#endif


int configureNetworkRoutes(int sockfd)
{
    printf("Configuring network routes... ");
    DIR *networkRoutesDir = opendir(NETWORK_ROUTES_DIR);
    if (!networkRoutesDir) {
        printf("init: failed to open %s directory.\n", NETWORK_ROUTES_DIR);
        return -1;
    }
    struct dirent dirEntry;
    struct dirent *entry = NULL;

    while ((readdir_r(networkRoutesDir, &dirEntry, &entry) == 0) && (entry != NULL)) {
         //TODO: use openat
        char buf[255];
        sprintf(buf, "%s%s", NETWORK_ROUTES_DIR, dirEntry.d_name);

        if (dirEntry.d_name[0] != '.') {
            StoredObject routeObj = StoredObject::loadObject(buf);
            if (!routeObj.isValid()) {
                printf("init: invalid route file: %s\n", buf);
                continue;
            }
            const char *routeDestination = routeObj.value("destination", (const char *) NULL);
            const char *routeGateway = routeObj.value("gateway", (const char *) NULL);
            const char *routeGenmask = routeObj.value("genmask", (const char *) NULL);

            if (!routeDestination || !routeGateway || !routeGenmask) {
                printf("init: cannot load %s: missing route property.\n", buf);
            }

            struct rtentry route;
            struct sockaddr_in sai;
            if (!inet_pton(0, routeDestination, &sai.sin_addr.s_addr)) {
                printf("init: %s invalid destination address\n", buf);
            }
            memcpy(&route.rt_dst, &sai, sizeof(struct sockaddr));
            if (!inet_pton(0, routeGateway, &sai.sin_addr.s_addr)) {
                printf("init: %s invalid gateway address\n", buf);
            }
            memcpy(&route.rt_gateway, &sai, sizeof(struct sockaddr));
            if (!inet_pton(0, routeGenmask, &sai.sin_addr.s_addr)) {
                printf("init: %s invalid genmask\n", buf);
            }
            memcpy(&route.rt_genmask, &sai, sizeof(struct sockaddr));

            route.rt_dev = "eth0";
            ioctl(sockfd, SIOCADDRT, &route);
        }
    }
    closedir(networkRoutesDir);
    printf("\t[ OK ]\n");
}

int configureNetwork()
{
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    configureNetworkInterfaces(sockfd);
    configureNetworkRoutes(sockfd);

    close(sockfd);
}

void setHostName()
{
    printf("Setting host name...");

    int hostnameFd = open("/etc/hostname", O_RDONLY | O_CLOEXEC, 0);
    if (hostnameFd < 0) {
        perror("init: cannot read /etc/hostname");
        return;
    }

    char hname[256];
    int readBytes = read(hostnameFd, hname, 256);

    close(hostnameFd);

    sethostname(hname, readBytes);
    struct utsname unm;
    uname(&unm);

    printf("\t[  %s  ]\n", unm.nodename);
}

void setDomainName()
{
    printf("Setting domain name...");

    int domainnameFd = open("/etc/domainname", O_RDONLY | O_CLOEXEC, 0);

    if (domainnameFd < 0) {
        perror("init: cannot read /etc/domainname");
        return;
    }

    char dname[256];
    int readBytes = read(domainnameFd, dname, 256);

    close(domainnameFd);

    setdomainname(dname, readBytes);
    struct utsname unm;
    uname(&unm);

    printf("\t[  %s  ]\n", unm.domainname);
}
