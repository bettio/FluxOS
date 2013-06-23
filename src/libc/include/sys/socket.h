/***************************************************************************
 *   Copyright 2013 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: socket.h                                                        *
 ***************************************************************************/

#ifndef _LIBC_SYS_SOCKET_H_
#define _LIBC_SYS_SOCKET_H_

//socket domains
#define PF_INET 2
#define PF_INET6 10

//socket types
#define SOCK_STREAM 1
#define SOCK_DGRAM 2

#if __cplusplus
extern "C"
{
#endif

struct in_addr { 
    uint32_t s_addr;
}; 

struct sockaddr
{
    short int sa_family;
    char sa_data[14];
};

struct sockaddr_in { 
     short int sin_family;
     short int sin_port;
     struct in_addr sin_addr;
};

struct in6_addr {
    uint8_t  s6_addr[16];
};

struct sockaddr_in6 {
    uint8_t sin6_len;
    uint8_t  sin6_family;
    uint16_t sin6_port;
    uint32_t sin6_flowinfo;
    struct in6_addr sin6_addr;
};

int socket(int domain, int type, int protocol);

#if __cplusplus
}
#endif

#endif

