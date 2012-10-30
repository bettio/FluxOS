/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: fsmoduleinfo.h                                                  *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#ifndef _FILESYSTEM_FSMODULEINFO_H_
#define _FILESYSTEM_FSMODULEINFO_H_

#include <filesystem/fstypes.h>

#include <QHash>

#include <kdef.h>

class VNode;
struct dirent;
class FileDescriptor;

struct FSModuleInfo
{
    MUST_CHECK int (*umount)(VNode *root);

    MUST_CHECK int (*openfd)(VNode *node, FileDescriptor *fdesc);
    MUST_CHECK int (*closefd)(VNode *node, FileDescriptor *fdesc);
    MUST_CHECK int (*dupfd)(VNode *node, FileDescriptor *fdesc);
    MUST_CHECK int (*closevnode)(VNode *node);

    MUST_CHECK int (*socketcall)(VNode *node, int call, unsigned long *args);

    MUST_CHECK int (*lookup)(VNode *node, const char *name, VNode **vnd, unsigned int *ntype);
    MUST_CHECK int (*read)(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
    MUST_CHECK int (*readlink)(VNode *linknode, char *buf, size_t bufsize);
    MUST_CHECK int (*write)(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize);
    MUST_CHECK int (*getdents)(VNode *node, dirent *dirp, unsigned int count);
    MUST_CHECK int (*stat)(VNode *node, struct stat *buf);
    MUST_CHECK int (*size)(VNode *node, int64_t *size);
    MUST_CHECK int (*type)(VNode *node, int *type);
    MUST_CHECK int (*name)(VNode *directory, VNode *node, char **name, int *len);
    MUST_CHECK int (*access)(VNode *node, int mode, int uid, int gid);
    MUST_CHECK int (*chmod)(VNode *node, mode_t mode);
    MUST_CHECK int (*chown)(VNode *node, uid_t uid, gid_t gid);
    MUST_CHECK int (*link)(VNode *directory, VNode *oldNode, const char *newName);
    MUST_CHECK int (*symlink)(VNode *directory, const char *oldName, const char *newName);
    MUST_CHECK int (*rename)(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName);
    MUST_CHECK int (*mknod)(VNode *directory, const char *newName, mode_t mode, dev_t dev);
    MUST_CHECK int (*mkdir)(VNode *directory, const char *newName, mode_t mode);
    MUST_CHECK int (*truncate)(VNode *node, uint64_t length);
    MUST_CHECK int (*fsync)(VNode *node);
    MUST_CHECK int (*fdatasync)(VNode *node);
    MUST_CHECK int (*unlink)(VNode *directory, const char *name);
    MUST_CHECK int (*rmdir)(VNode *directory, const char *name);
    MUST_CHECK int (*creat)(VNode *directory, const char *name, mode_t mode);
    MUST_CHECK int (*statfs)(VNode *directory, struct statfs *buf);
    MUST_CHECK int (*utime)(VNode *node, const struct utimbuf *buf);
    MUST_CHECK int (*fcntl)(VNode *node, int cmd, long arg);
    MUST_CHECK int (*ioctl)(VNode *node, int request, long arg);
    MUST_CHECK void *(*mmap)(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset); //TODO -Eerror?
};

#endif
