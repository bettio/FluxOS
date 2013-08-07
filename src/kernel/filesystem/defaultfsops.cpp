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
 *   Name: defaultfsops.cpp                                                *
 *   Date: 04/11/2012                                                      *
 ***************************************************************************/

#include <filesystem/defaultfsops.h>

using namespace FileSystem;

int DefaultFSOps::accept(VNode *socknode, struct sockaddr *addr, int *addrlen, VNode **newEndPoint)
{
    return -ENOTSOCK;
}

int DefaultFSOps::bind(VNode *socknode, const struct sockaddr *addr, int addrlen)
{
    return -ENOTSOCK;
}

int DefaultFSOps::connect(VNode *socknode, const struct sockaddr *addr, int addrlen)
{
    return -ENOTSOCK;
}

int DefaultFSOps::getpeername(VNode *socknode, struct sockaddr *addr, int *addrlen)
{
    return -ENOTSOCK;
}

int DefaultFSOps::getsockname(VNode *socknode, struct sockaddr *addr, int *addrlen)
{
    return -ENOTSOCK;
}

int DefaultFSOps::getsockopt(VNode *socknode, int level, int optname, void *optval, int *optlen)
{
    return -ENOTSOCK;
}

int DefaultFSOps::listen(VNode *socknode, int backlog)
{
    return -ENOTSOCK;
}

int DefaultFSOps::recv(VNode *socknode, void *buf, size_t len, int flags)
{
    return -ENOTSOCK;
}

int DefaultFSOps::recvfrom(VNode *socknode, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen)
{
    return -ENOTSOCK;
}

int DefaultFSOps::recvmsg(VNode *socknode, struct msghdr *msg, int flags)
{
    return -ENOTSOCK;
}

int DefaultFSOps::send(VNode *socknode, const void *buf, size_t len, int flags)
{
    return -ENOTSOCK;
}

int DefaultFSOps::sendmsg(VNode *socknode, const struct msghdr *msg, int flags)
{
    return -ENOTSOCK;
}

int DefaultFSOps::sendto(VNode *socknode, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, int addrlen)
{
    return -ENOTSOCK;
}

int DefaultFSOps::setsockopt(VNode *socknode, int level, int optname, const void *optval, int optlen)
{
    return -ENOTSOCK;
}

int DefaultFSOps::shutdown(VNode *socknode, int how)
{
    return -ENOTSOCK;
}

int DefaultFSOps::umount(VNode *root)
{
    return -EINVAL;
}

int DefaultFSOps::lookup(VNode *node, const char *name,VNode **vnd, unsigned int *ntype)
{
    return -EINVAL;
}

int DefaultFSOps::readlink(VNode *node, char *buffer, size_t bufsize)
{
    return -EINVAL;
}

int DefaultFSOps::getdents(VNode *node, dirent *dirp, unsigned int count)
{
    return -EINVAL;
}

int DefaultFSOps::stat(VNode *node, struct stat *buf)
{
    return -EINVAL;
}

int DefaultFSOps::access(VNode *node, int aMode, int uid, int gid)
{
    return -EINVAL;
}

int DefaultFSOps::name(VNode *directory, VNode *node, char **name, int *len)
{
    return -EINVAL;
}


int DefaultFSOps::chmod(VNode *node, mode_t mode)
{
    return -EINVAL;
}

int DefaultFSOps::chown(VNode *node, uid_t uid, gid_t gid)
{
    return -EINVAL;
}

int DefaultFSOps::link(VNode *directory, VNode *oldNode, const char *newName)
{
    return -EINVAL;
}

int DefaultFSOps::symlink(VNode *directory, const char *oldName, const char *newName)
{
    return -EINVAL;
}

int DefaultFSOps::rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName)
{
    return -EINVAL;
}

int DefaultFSOps::mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev)
{
    return -EINVAL;
}

int DefaultFSOps::mkdir(VNode *directory, const char *newName, mode_t mode)
{
    return -EINVAL;
}

int DefaultFSOps::truncate(VNode *node, uint64_t length)
{
    return -EINVAL;
}

int DefaultFSOps::fsync(VNode *node)
{
    return -EINVAL;
}

int DefaultFSOps::fdatasync(VNode *node)
{
    return -EINVAL;
}

int DefaultFSOps::unlink(VNode *directory, const char *name)
{
    return -EINVAL;
}

int DefaultFSOps::rmdir(VNode *directory, const char *name)
{
    return -EINVAL;
}

int DefaultFSOps::creat(VNode *directory, const char *name, mode_t mode)
{
    return -EINVAL;
}

int DefaultFSOps::statfs(VNode *directory, struct statfs *buf)
{
    return -EINVAL;
}

int DefaultFSOps::size(VNode *node, int64_t *size)
{
    return -EINVAL;
}

int DefaultFSOps::utime(VNode *node, const struct utimbuf *buf)
{
    return -EINVAL;
}

void *DefaultFSOps::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return 0;
}


