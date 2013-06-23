/***************************************************************************
 *   Copyright 2012 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: if.h                                                            *
 *   Date: 06/11/2012                                                      *
 ***************************************************************************/

#ifndef _NET_IF_H_
#define _NET_IF_H_

#include <sys/socket.h>

//routing ioctls
#define SIOCADDRT           0x890B

//interface ioctls
#define SIOCGIFNAME         0x8910 
#define SIOCGIFCONF         0x8912
#define SIOCGIFFLAGS        0x8913
#define SIOCSIFFLAGS        0x8914
#define SIOCGIFADDR         0x8915
#define SIOCSIFADDR         0x8916
#define SIOCGIFNETMASK      0x891B
#define SIOCSIFNETMASK      0x891C

#define IFNAMSIZ 16

#define IFF_UP          0x1 
#define IFF_RUNNING     0x40

//ifmap, ifreq, ifconf: they must be the same as the Linux one for compatibility reasons

struct ifmap
{
    unsigned long int mem_start;
    unsigned long int mem_end;
    unsigned short int base_addr;
    unsigned char irq;
    unsigned char dma;
    unsigned char port;
};

struct ifreq {
    char ifr_name[IFNAMSIZ];
    union {
        struct sockaddr ifr_addr;
        struct sockaddr ifr_dstaddr;
        struct sockaddr ifr_broadaddr;
        struct sockaddr ifr_netmask;
        struct sockaddr ifr_hwaddr;
        short ifr_flags;
        int ifr_ifindex;
        int ifr_metric;
        int ifr_mtu;
        struct ifmap ifr_map;
        char ifr_slave[IFNAMSIZ];
        char ifr_newname[IFNAMSIZ];
        char           *ifr_data;
    };
};

struct ifconf
{
    int ifc_len;
    union {
        char *ifc_buf;
        struct ifreq *ifc_req;
    };
};

#endif

