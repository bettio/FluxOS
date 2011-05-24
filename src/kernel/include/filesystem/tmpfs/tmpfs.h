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
            static int Umount(VNode *root);
            static int Lookup(VNode *node, const char *name,VNode **vnd, unsigned int *ntype);
            static int CloseVNode(VNode *node);
            static int Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
            static int Readlink(VNode *node, char *buffer, size_t bufsize);
            static int Write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize);
            static int GetDEnts(VNode *node, dirent *dirp, unsigned int count);
            static int Stat(VNode *node, struct stat *buf);
            static int Access(VNode *node, int aMode, int uid, int gid);
            static int Name(VNode *directory, VNode *node, char **name, int *len);
            static int Chmod(VNode *node, mode_t mode);
            static int Chown(VNode *node, uid_t uid, gid_t gid);
            static int Link(VNode *directory, VNode *oldNode, const char *newName);
            static int Symlink(VNode *directory, const char *oldName, const char *newName);
            static int Rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName);
            static int Mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev);
            static int Mkdir(VNode *directory, const char *newName, mode_t mode);
            static int Truncate(VNode *node, uint64_t length);
            static int FSync(VNode *node);
            static int FDataSync(VNode *node);
            static int Unlink(VNode *directory, const char *name);
            static int Rmdir(VNode *directory, const char *name);
            static int Creat(VNode *directory, const char *name, mode_t mode);
            static int Creat(VNode *directory, const char *name, mode_t mode, TmpInode **inode);
            static int StatFS(VNode *directory, struct statfs *buf);
            static int Utime(VNode *node, const struct utimbuf *buf);
            static int Fcntl(VNode *node, int cmd, long arg);
            static int Ioctl(VNode *node, int request, long arg);
            static void *Mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset);
            
    private:
            static FSModuleInfo *NewFSModuleInfo();
    };
}

#endif
