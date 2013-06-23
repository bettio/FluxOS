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
 *   Name: ip6socketcalls.h                                                *
 *   Date: 05/11/2012                                                      *
 ***************************************************************************/

#ifndef _NET_IP6SOCKETCALLS_H_
#define _NET_IP6SOCKETCALLS_H_

class IP6SocketCalls {
    public:
        static int bindToSocket(VNode *node, int domain, int type, int protocol);

        static int accept(VNode *socknode, struct sockaddr *addr, int *addrlen);
        static int bind(VNode *socknode, const struct sockaddr *addr, int addrlen);
        static int connect(VNode *socknode, const struct sockaddr *addr, int addrlen);
        static int getpeername(VNode *socknode, struct sockaddr *addr, int *addrlen);
        static int getsockname(VNode *socknode, struct sockaddr *addr, int *addrlen);
        static int getsockopt(VNode *socknode, int level, int optname, void *optval, int *optlen);
        static int listen(VNode *socknode, int backlog);
        static int recv(VNode *socknode, void *buf, size_t len, int flags);
        static int recvfrom(VNode *socknode, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen);
        static int recvmsg(VNode *socknode, struct msghdr *msg, int flags);
        static int  send(VNode *socknode, const void *buf, size_t len, int flags);
        static int sendmsg(VNode *socknode, const struct msghdr *msg, int flags);
        static int sendto(VNode *socknode, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, int addrlen);
        static int setsockopt(VNode *socknode, int level, int optname, const void *optval, int optlen);
        static int shutdown(VNode *socknode, int how);

        static int openfd(VNode *node, FileDescriptor *fdesc);
        static int closefd(VNode *node, FileDescriptor *fdesc);
        static int dupfd(VNode *node, FileDescriptor *fdesc);
        static int closevnode(VNode *node);
        static int read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
        static int write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize);
        static int type(VNode *node, int *type); 
        static int fcntl(VNode *node, int cmd, long arg);
        static int ioctl(VNode *node, int request, long arg);

    private:
       static FSModuleInfo calls;
       static FSMount ipSocketFakeMount;
};

#endif

