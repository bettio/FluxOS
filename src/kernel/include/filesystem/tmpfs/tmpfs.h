/***************************************************************************
 *   Copyright 2008 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: tmpfs.h                                                         *
 *   Date: 10/06/2008                                                      *
 ***************************************************************************/

#ifndef _FILESYSTEM_TMPFS_TMPFS_H_
#define _FILESYSTEM_TMPFS_TMPFS_H_

#include <filesystem/vfs.h>
#include <QList>

struct TmpInode
{
    uint32_t Id;
    uint16_t Mode;
    uint16_t Uid;
    uint32_t Size;
    uint32_t ATime;
    uint32_t CTime;
    uint32_t MTime;
    uint16_t Gid;
    uint16_t LinksCount;
    uint32_t Blocks;
    uint32_t Flags;

    uint32_t Major;
    uint32_t Minor;

    uint8_t *FileData;
    QHash<QString, int> Directory;
};


struct TmpFSPrivData
{
    QList<TmpInode *> Inodes;
};

namespace FileSystem
{
    class TmpFS
    {
        public:
            static int Init();
            static int Mount(FSMount *fsmount, BlockDevice *blkdev);
            static int umount(VNode *root);
            static int socketcall(VNode *node, int call, unsigned long *args);
            static int openfd(VNode *node, FileDescriptor *fdesc);
            static int closefd(VNode *node, FileDescriptor *fdesc);
            static int dupfd(VNode *node, FileDescriptor *fdesc);	
            static int lookup(VNode *node, const char *name,VNode **vnd, unsigned int *ntype);
            static int closevnode(VNode *node);
            static int read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
            static int readlink(VNode *node, char *buffer, size_t bufsize);
            static int write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize);
            static int getdents(VNode *node, dirent *dirp, unsigned int count);
            static int stat(VNode *node, struct stat *buf);
            static int access(VNode *node, int aMode, int uid, int gid);
            static int name(VNode *directory, VNode *node, char **name, int *len);
            static int chmod(VNode *node, mode_t mode);
            static int chown(VNode *node, uid_t uid, gid_t gid);
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
            static int creat(VNode *directory, const char *name, mode_t mode, TmpInode **inode);
            static int statfs(VNode *directory, struct statfs *buf);
            static int size(VNode *node, int64_t *size);
            static int type(VNode *node, int *type); 
            static int utime(VNode *node, const struct utimbuf *buf);
            static int fcntl(VNode *node, int cmd, long arg);
            static int ioctl(VNode *node, int request, long arg);
            static void *mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset);
            
    private:
            static FSModuleInfo calls;
    };
}

#endif
