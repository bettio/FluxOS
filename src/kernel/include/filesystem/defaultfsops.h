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
 *   Name: defaultfsops.h                                                  *
 *   Date: 04/11/2012                                                      *
 ***************************************************************************/

#ifndef _FILESYSTEM_DEFAULTFSOPS_H_
#define _FILESYSTEM_DEFAULTFSOPS_H_

#include <filesystem/vfs.h>

struct sockaddr;
struct msghdr;
struct dirent;
struct stat;
struct statfs;
struct utimbuf;

namespace FileSystem
{
    class DefaultFSOps
    {
        public:
            static int lookup(VNode *node, const char *name,VNode **vnd, unsigned int *ntype);
            static int readlink(VNode *node, char *buffer, size_t bufsize);
            static int getdents(VNode *node, dirent *dirp, unsigned int count);
            static int stat(VNode *node, struct stat *buf);
            static int access(VNode *node, int aMode, int uid, int gid);
            static int size(VNode *node, int64_t *size);
			static int name(VNode *directory, VNode *node, char **name, int *len);
			static int chmod(VNode *node, mode_t mode);
            static int chown(VNode *node, uid_t uid, gid_t gid);
            static int umount(VNode *root);
            static int link(VNode *directory, VNode *oldNode, const char *newName);
            static int symlink(VNode *directory, const char *oldName, const char *newName);
            static int rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName);
            static int mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev);
            static int mkdir(VNode *directory, const char *newName, mode_t mode);
            static int truncate(VNode *node, uint64_t length);
            static int fsync(VNode *node);
            static int fdatasync(VNode *node);
            static int unlink(VNode *directory, const char *name);
            static int rmdir(VNode *directory, const char *name);
            static int creat(VNode *directory, const char *name, mode_t mode);
            static int statfs(VNode *directory, struct statfs *buf);
            static int utime(VNode *node, const struct utimbuf *buf);
            static void *mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset);


            static int accept(VNode *socknode, struct sockaddr *addr, int *addrlen, VNode **newEndPoint);
            static int bind(VNode *socknode, const struct sockaddr *addr, int addrlen);
            static int connect(VNode *socknode, const struct sockaddr *addr, int addrlen);
            static int getpeername(VNode *socknode, struct sockaddr *addr, int *addrlen);
            static int getsockname(VNode *socknode, struct sockaddr *addr, int *addrlen);
            static int getsockopt(VNode *socknode, int level, int optname, void *optval, int *optlen);
            static int listen(VNode *socknode, int backlog);
            static int recv(VNode *socknode, void *buf, size_t len, int flags);
            static int recvfrom(VNode *socknode, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen);
            static int recvmsg(VNode *socknode, struct msghdr *msg, int flags);
            static int send(VNode *socknode, const void *buf, size_t len, int flags);
            static int sendmsg(VNode *socknode, const struct msghdr *msg, int flags);
            static int sendto(VNode *socknode, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, int addrlen);
            static int setsockopt(VNode *socknode, int level, int optname, const void *optval, int optlen);
            static int shutdown(VNode *socknode, int how);
    };
}

#endif

