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
 *   Name: tmpfs.cpp                                                       *
 *   Date: 10/06/2008                                                      *
 ***************************************************************************/

#include <filesystem/tmpfs/tmpfs.h>

#ifdef USE_GLIBC
#include <stdlib.h>
#endif
#include <string.h>

#include <core/systemtimer.h>
#include <core/printk.h>

#include <filesystem/vfs.h>
#include <filesystem/vnodemanager.h>
#include <filesystem/utimbuf.h>
#include <filesystem/statfs.h>

#include <QList>
#include <QHash>
#include <QString>

using namespace FileSystem;

FSCALLS(TmpFS::calls)

inline TmpInode *Inode(const VNode *node)
{
    TmpFSPrivData *privdata = (TmpFSPrivData *) node->mount->privdata;
    return privdata->Inodes[node->vnid.id];
}

int TmpFS::Init()
{
    FileSystemInfo *info = new FileSystemInfo;
    if (info == NULL) return -ENOMEM;

    info->name = "tmpfs";
    info->mount = Mount;

    FileSystem::VFS::RegisterFileSystem(info);

    return 0;
}

int TmpFS::Mount(FSMount *fsmount, BlockDevice *)
{
    fsmount->fs = &calls;

    TmpFSPrivData *privdata = new TmpFSPrivData;
    if (privdata == NULL){
        delete fsmount->fs;
        return -ENOMEM;
    }
    fsmount->privdata = (void *) privdata;
    privdata->Inodes = QList<TmpInode *>();

    TmpInode *tmpInode = new TmpInode;
    tmpInode->FileData = 0;
    tmpInode->Mode = S_IFDIR;
    tmpInode->Uid = 0;
    tmpInode->Gid = 0;
    tmpInode->ATime = SystemTimer::time() * 1000;
    tmpInode->MTime = SystemTimer::time() * 1000;
    tmpInode->CTime = SystemTimer::time() * 1000;
    tmpInode->LinksCount = 2;
    tmpInode->Directory = QHash<QString, int>();
    tmpInode->Directory.insert(".", 1);
    tmpInode->Directory.insert("..", 1);
    privdata->Inodes.append(NULL); //The first inode is the inode 1
    privdata->Inodes.append(tmpInode);

    VNode *root;
    VNodeManager::GetVnode(fsmount->mountId, 1, &root);
    root->mount = fsmount;
    root->vnid.id = 1;
    root->vnid.mountId = fsmount->mountId;
    fsmount->fsRootVNode = root;

    return 0; //TODO: We must return a good value
}

int TmpFS::umount(VNode *root)
{
    TmpFSPrivData *privdata = (TmpFSPrivData *) root->mount->privdata;

    for (int i = 0; i < privdata->Inodes.count(); i++){
        if (privdata->Inodes[i] && privdata->Inodes[i]->FileData){
            free(privdata->Inodes[i]->FileData);
        }
        delete privdata->Inodes[i];
    }
    
    delete privdata;
    delete root->mount->fs;
    root->mount = 0;
    
    return 0; //TODO: We must return a good value
}

int TmpFS::socketcall(VNode *node, int call, unsigned long *args)
{
    return -EINVAL;
}

int TmpFS::openfd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int TmpFS::closefd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int TmpFS::dupfd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int TmpFS::lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
    TmpFSPrivData *privdata = (TmpFSPrivData *) node->mount->privdata;
    TmpInode *inode = privdata->Inodes[node->vnid.id];

    if (!S_ISDIR(inode->Mode)) return -ENOTDIR;

    int inodeIndex = inode->Directory.value(name);
    if (inodeIndex){
        VNodeManager::GetVnode(node->mount->mountId, inodeIndex, vnd);

        (*vnd)->mount = node->mount;
        *ntype = privdata->Inodes[inodeIndex]->Mode;

#if 0
		if (S_ISCHR(*ntype)){
			printk("Char device\n");
			FSMount *mnt = new FSMount;
			memcpy(mnt, node->mount, sizeof(FSMount));
			FSModuleInfo *modInfo = new FSModuleInfo;
			memcpy(modInfo, mnt->fs, sizeof(FSModuleInfo));
			
			mnt->fs = modInfo;
			(*vnd)->mount = mnt;

			char_device *dev = CharDeviceManager::CharDevice(privdata->Inodes[inodeIndex]->Major, privdata->Inodes[inodeIndex]->Minor);
			printk("Major: %i, Minor: %i\n", privdata->Inodes[inodeIndex]->Major, privdata->Inodes[inodeIndex]->Minor);
			modInfo->read = dev->read;
			modInfo->write = dev->write;
			modInfo->ioctl = dev->ioctl;
		}
#endif
        return 0;

    }else{
        vnd = NULL;
        printk("ENOENT: %s (node addr: %lx)\n", name, (unsigned long) node);
        return -ENOENT;
    }
}

int TmpFS::closevnode(VNode *node)
{
    return 0;
}

int TmpFS::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    TmpInode *inode = Inode(node);

    if (S_ISDIR(inode->Mode)) return -EISDIR;

    if (bufsize + pos > inode->Size) bufsize = inode->Size - pos;
    memcpy(buffer, inode->FileData + pos, bufsize);

    return bufsize;
}

int TmpFS::readlink(VNode *node, char *buffer, size_t bufsize)
{
    TmpInode *inode = Inode(node);

    if (!S_ISLNK(inode->Mode)) return -EINVAL;

    if (bufsize > inode->Size) bufsize = inode->Size;
    memcpy(buffer, inode->FileData, bufsize);

    return bufsize;
}

int TmpFS::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize, WriteOpFlags flags)
{   
    TmpInode *inode = Inode(node);

    if (S_ISDIR(inode->Mode)) return -EISDIR;  //TODO: Check&Kill: what about links

    //TODO: effettuare controlli aggiuntivi, del tipo: la scrittura e` oltre la fine?
    if (bufsize + pos > inode->Size){
        int ret = truncate(node, bufsize + pos);
        if (ret < 0) return ret;
    }

    memcpy(inode->FileData + pos, buffer, bufsize);

    return bufsize;
}

//TODO: it doesn't work if the directory is completely empty (without . and ..)
int TmpFS::getdents(VNode *node, dirent *dirp, unsigned int count)
{
    TmpInode *inode = Inode(node);

    if (!S_ISDIR(inode->Mode)) return -ENOTDIR;

    QHash<QString, int>::const_iterator dirIterator = inode->Directory.constBegin();
    unsigned int bufferUsedBytes = 0;
    do{
        rawstrcpy(dirp->d_name, dirIterator.key().toAscii(), sizeof(dirp->d_name), dirIterator.key().length() + 1);
        dirp->d_reclen = sizeof(dirent);
        dirp->d_off = sizeof(dirent); //TODO: ci andrebbe pos
        bufferUsedBytes += dirp->d_reclen;

        dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen);
        ++dirIterator;
    }while ((dirIterator != inode->Directory.constEnd()) && (bufferUsedBytes + sizeof(dirent) < count));

    return bufferUsedBytes;
}

int TmpFS::stat(VNode *node, struct stat *buf)
{
    TmpInode *inode = Inode(node);

    buf->st_dev = 0;
    buf->st_ino = node->vnid.id;
    buf->st_mode = inode->Mode;
    buf->st_nlink = inode->LinksCount;
    buf->st_uid = inode->Uid;
    buf->st_gid = inode->Gid;
    buf->st_rdev = 0;
    buf->st_size = inode->Size;
    buf->st_blksize = 1024; //TODO: pagesize?
    buf->st_blocks = inode->Blocks;
    buf->st_atime = inode->ATime;
    buf->st_mtime = inode->MTime;
    buf->st_ctime = inode->CTime;

    return 0;
}

int TmpFS::access(VNode *node, int aMode, int uid, int gid)
{
    TmpInode *inode = Inode(node);

    //Write only
    //No execute

    mode_t tmpIMode = inode->Mode;

    if (uid == 0){
        tmpIMode |= (06 | ((tmpIMode >> 14) & 1));

    }else if (uid == inode->Uid){
        tmpIMode >>= 6;

    }else if (gid == inode->Gid){
        tmpIMode >>= 3;
    }

    if (aMode & ~(tmpIMode & 07)) return -EACCES;

    return 0;
}

int TmpFS::chmod(VNode *node, mode_t mode)
{
    TmpInode *inode = Inode(node);

    inode->Mode = mode;

    return 0;
}

int TmpFS::chown(VNode *node, uid_t uid, gid_t gid)
{
    TmpInode *inode = Inode(node);

    inode->Uid = uid;
    inode->Gid = gid;

    return 0;
}

//TODO: We must do some checks
int TmpFS::link(VNode *directory, VNode *oldNode, const char *newName)
{
    TmpInode *inode = Inode(directory);

    inode->Directory.insert(newName, oldNode->vnid.id);

    return 0;
}

int TmpFS::symlink(VNode *directory, const char *oldName, const char *newName)
{
    TmpInode *tmpInode;
    int retVal = creat(directory, newName, 0, &tmpInode);
    if (retVal) return retVal;

    tmpInode->FileData = (uint8_t *) strdup(oldName);
    tmpInode->Size = strlen(oldName) + 1;
    tmpInode->Mode = S_IFLNK;
    
    return 0;
}

int TmpFS::rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName)
{
    TmpInode *oldInode = Inode(oldDirectory);
    int inodeId = oldInode->Directory.value(oldName);
    oldInode->Directory.remove(oldName);
    
    TmpInode *inode = Inode(newDirectory);
    inode->Directory.insert(newName, inodeId);

    return 0;
}

int TmpFS::mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev)
{
    TmpInode *tmpInode;
    int retVal = creat(directory, newName, mode, &tmpInode);
    if (retVal) return retVal;

    tmpInode->Major = dev >> 16; //TODO: Need to be changed to 64 bit
    tmpInode->Minor = dev & 0xFFFF; //TODO: Need to be changed to 64 bit
    tmpInode->FileData = 0;

    return 0;
}

int TmpFS::mkdir(VNode *directory, const char *newName, mode_t mode)
{
    TmpInode *tmpInode;
    int retVal = creat(directory, newName, mode, &tmpInode);
    if (retVal) return retVal;
    
    tmpInode->FileData = 0;
    tmpInode->Mode = S_IFDIR;
    tmpInode->Directory = QHash<QString, int>();
    tmpInode->Directory.insert(".", tmpInode->Id);
    tmpInode->Directory.insert("..", directory->vnid.id);
    
    return 0;
}

int TmpFS::truncate(VNode *node, uint64_t length)
{
    TmpInode *inode = Inode(node);

    uint8_t *tmpPtr = (uint8_t *) realloc(inode->FileData, length);
    if (tmpPtr == NULL) return -ENOSPC;
    inode->FileData = tmpPtr;
    if (length > inode->Size){
        int startOfNewArea = (inode->Size > 0) ? inode->Size - 1 : 0;
        memset(tmpPtr + startOfNewArea, 0, length - inode->Size);
    }
    inode->Size = length;

    return 0;
}

int TmpFS::fsync(VNode *node)
{
    return 0;
}

int TmpFS::fdatasync(VNode *node)
{
    return 0;
}

int TmpFS::unlink(VNode *directory, const char *name)
{
    TmpInode *inode = Inode(directory);
    
    //TODO: questo if e` un evvidente errore sematico,
    //questo lavoro deve essere fatto da rmdir
    if (inode->Directory.remove(name)){
        //TODO: Cancellare dati ed inode nel caso
    
        return 0;

    }else{
        return -ENOENT;
    }
}

int TmpFS::rmdir(VNode *directory, const char *name)
{
    //TODO: check directory
    
    return unlink(directory, name);
}

int TmpFS::creat(VNode *directory, const char *name, mode_t mode, TmpInode **tmpInode)
{
    TmpFSPrivData *privdata = (TmpFSPrivData *) directory->mount->privdata;

    *tmpInode = new TmpInode;
    (*tmpInode)->Mode = mode;
    (*tmpInode)->Size = 0;
    (*tmpInode)->Uid = 0;
    (*tmpInode)->Gid = 0;
    (*tmpInode)->ATime = SystemTimer::time() * 1000;
    (*tmpInode)->MTime = SystemTimer::time() * 1000;
    (*tmpInode)->CTime = SystemTimer::time() * 1000;
    (*tmpInode)->LinksCount = 1;

    privdata->Inodes.append(*tmpInode);
    (*tmpInode)->Id = privdata->Inodes.count() - 1;

    //TODO: strdup
    privdata->Inodes[directory->vnid.id]->Directory.insert(name, (*tmpInode)->Id);

    return 0;
}

int TmpFS::creat(VNode *directory, const char *name, mode_t mode)
{
    TmpInode *tmpInode;
    int retVal = creat(directory, name, mode, &tmpInode);
    if (retVal) return retVal;
    
    tmpInode->FileData = (uint8_t *) malloc(4);

    return 0;
}

//????
int TmpFS::name(VNode *directory, VNode *node, char **name, int *len)
{
    TmpFSPrivData *privdata = (TmpFSPrivData *) directory->mount->privdata;
    TmpInode *inode = privdata->Inodes[directory->vnid.id];

    if (!S_ISDIR(inode->Mode)) return -ENOTDIR;

    QHash<QString, int>::const_iterator dirIterator = inode->Directory.constBegin();
    do{
        if ((unsigned int) dirIterator.value() == node->vnid.id){
            *name = strndup(dirIterator.key().toAscii(), dirIterator.key().length() + 1);
            *len = dirIterator.key().length();
            return 0;
        }
        dirIterator++;
    }while (dirIterator != inode->Directory.constEnd());

    return 0;
}

int TmpFS::statfs(VNode *directory, struct statfs *buf)
{
    buf->f_type = -1; //TODO
    buf->f_bsize = -1; //TODO
    buf->f_blocks = -1; //TODO
    buf->f_bfree = -1; //TODO
    buf->f_bavail = -1; //TODO
    buf->f_files = -1 ; //TODO
    buf->f_ffree = -1;
    //f_fsid;
    buf->f_namelen = 256;
  
    return 0;
}

int TmpFS::size(VNode *node, int64_t *size)
{
    TmpInode *inode = Inode(node);
    *size = inode->Size;

    return 0;
}

int TmpFS::type(VNode *node, int *type)
{
    TmpInode *inode = Inode(node);
    *type = inode->Mode & S_IFMT;

    return 0;
}

int TmpFS::utime(VNode *node, const struct utimbuf *buf)
{
    TmpInode *inode = Inode(node);
    
    inode->ATime = buf->actime;
    inode->MTime = buf->modtime;
    
    return 0;
}

int TmpFS::fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int TmpFS::ioctl(VNode *node, int request, long arg)
{
    return -EIOCTLNOTSUPPORTED;
}

//HACK: it should map memory!
void *TmpFS::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    TmpInode *inode = Inode(node);
    if (S_ISDIR(inode->Mode)) return (void *) -EISDIR;

    return inode->FileData;
}
