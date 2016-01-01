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
 *   Name: netcalls.h                                                      *
 *   Date: 11/10/2012                                                      *
 ***************************************************************************/

#ifndef _NETCALLS_H_
#define _NETCALLS_H_

#include <net/nettypes.h>

class NetUAPI
{
    public:
        static int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        static int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        static int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        static int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        static int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        static int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
        static int listen(int sockfd, int backlog);
        static ssize_t recv(int sockfd, void *buf, size_t len, int flags);
        static ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
        static ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
        static ssize_t send(int sockfd, const void *buf, size_t len, int flags);
        static ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
        static ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
        static int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
        static int shutdown(int sockfd, int how);
};

#endif

