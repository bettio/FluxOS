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
 *   Name: netcalls.cpp                                                    *
 ***************************************************************************/

#include <uapi/netuapi.h>

#include <cstdlib.h>
#include <core/printk.h>
#include <filesystem/filedescriptor.h>
#include <filesystem/vnode.h>
#include <filesystem/vnodemanager.h>
#include <filesystem/socket.h>
#include <net/nettypes.h>
#include <task/scheduler.h>

using namespace FileSystem;

#define CHECK_FOR_EBADF(fd) \
if (!((fd >= 0) && (fd < Scheduler::currentThread()->parentProcess->openFiles->size()))) return -EBADF;

int NetUAPI::accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    VNode *newNode;
    int retVal = FS_CALL(fdesc->node, accept)(fdesc->node, addr, addrlen, &newNode);
    if (retVal == 0){
        FileDescriptor *fdesc = new FileDescriptor(newNode);
        if (fdesc == NULL) return -ENOMEM;
        //fdesc->flags = flags;
        retVal = Scheduler::currentThread()->parentProcess->openFiles->add(fdesc);
    }
    return retVal;
}

int NetUAPI::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, bind)(fdesc->node, addr, addrlen);
}

int NetUAPI::connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, connect)(fdesc->node, addr, addrlen);
}

int NetUAPI::getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, getpeername)(fdesc->node, addr, addrlen);
}

int NetUAPI::getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, getsockname)(fdesc->node, addr, addrlen);
}

int NetUAPI::getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, getsockopt)(fdesc->node, level, optname, optval, optlen);
}

int NetUAPI::listen(int sockfd, int backlog)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, listen)(fdesc->node, backlog);
}

ssize_t NetUAPI::recv(int sockfd, void *buf, size_t len, int flags)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, recv)(fdesc->node, buf, len, flags);
}

ssize_t NetUAPI::recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, recvfrom)(fdesc->node, buf, len, flags, src_addr, addrlen);
}

ssize_t NetUAPI::recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, recvmsg)(fdesc->node, msg, flags);
}

ssize_t NetUAPI::send(int sockfd, const void *buf, size_t len, int flags)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, send)(fdesc->node, buf, len, flags);
}

ssize_t NetUAPI::sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, sendmsg)(fdesc->node, msg, flags);
}

ssize_t NetUAPI::sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, sendto)(fdesc->node, buf, len, flags, dest_addr, addrlen);
}

int NetUAPI::setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, setsockopt)(fdesc->node, level, optname, optval, optlen);
}

int NetUAPI::shutdown(int sockfd, int how)
{
    CHECK_FOR_EBADF(sockfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(sockfd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, shutdown)(fdesc->node, how);
}

