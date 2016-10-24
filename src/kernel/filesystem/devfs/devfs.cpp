/***************************************************************************
 *   Copyright 2007 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: devfs.cpp                                                       *
 *   Date: 20/05/2007                                                      *
 ***************************************************************************/

#include <kdef.h>
#include <core/printk.h>
#include <core/systemtimer.h>
#include <cstdlib.h>
#include <cstring.h>
#include <arch.h>
#include <QHash>
#include <QList>
#include <QString>

#include <filesystem/vfs.h>
#include <filesystem/devfs/devfs.h>
#include <filesystem/vnodemanager.h>
#include <drivers/chardevicemanager.h>
#include <drivers/blockdevicemanager.h>

struct TmpInode
{
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
    QHash<QString, int> *Directory;
};

using namespace FileSystem;

FSCALLS(DevFS::calls);
QList<TmpInode *> *Inodes;

int DevFS::Init()
{
    Inodes = new QList<TmpInode *>();
    TmpInode *tmpInode = new TmpInode;
    tmpInode->FileData = 0;
    tmpInode->Mode = S_IFDIR;
    tmpInode->Uid = 0;
    tmpInode->Gid = 0;
    tmpInode->ATime = SystemTimer::time() * 1000;
    tmpInode->MTime = SystemTimer::time() * 1000;
    tmpInode->CTime = SystemTimer::time() * 1000;
    tmpInode->LinksCount = 2;
    tmpInode->Directory = new QHash<QString, int>();
    tmpInode->Directory->insert(".", 1);
    tmpInode->Directory->insert("..", 1);
    Inodes->append(NULL); //The first inode is the inode 1
    Inodes->append(tmpInode);

    return 0;
}

int DevFS::RegisterAsFileSystem()
{
	FileSystemInfo *info = new FileSystemInfo;
	if (info == NULL) return -ENOMEM;
	info->name = "devfs";
	info->mount = Mount;

	return FileSystem::VFS::RegisterFileSystem(info);
}

int DevFS::Mount(FSMount *fsmount, BlockDevice *)
{
	fsmount->fs = &calls;

	VNode *root; 
	VNodeManager::GetVnode(fsmount->mountId, 1, &root);

	root->mount = fsmount;
	root->vnid.id = 1;
	root->vnid.mountId = fsmount->mountId;

	fsmount->fsRootVNode = root;

	//TODO: Ritornare un valore adeguato
	return 0;
}

int DevFS::umount(VNode *root)
{
     return -EINVAL;
}

int DevFS::socketcall(VNode *node, int call, unsigned long *args)
{
     return -EINVAL;
}

int DevFS::openfd(VNode *node, FileDescriptor *fdesc)
{
     return -EINVAL;
}

int DevFS::closefd(VNode *node, FileDescriptor *fdesc)
{
     return -EINVAL;
}

int DevFS::dupfd(VNode *node, FileDescriptor *fdesc)
{
     return -EINVAL;
}

int DevFS::closevnode(VNode *node)
{
     return 0;
}

int DevFS::lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
	TmpInode *inode = Inodes->at(node->vnid.id);

	if (!S_ISDIR(inode->Mode)) return -ENOTDIR;

	int inodeIndex = inode->Directory->value(name);

	if (inodeIndex){
		VNodeManager::GetVnode(node->mount->mountId, inodeIndex, vnd);

		(*vnd)->mount = node->mount;
		*ntype = Inodes->at(inodeIndex)->Mode;

		if (S_ISCHR(*ntype)){
			///printk("Char device\n");
			FSMount *mnt = new FSMount;
			memcpy(mnt, node->mount, sizeof(FSMount));
			FSModuleInfo *modInfo = new FSModuleInfo;
			memcpy(modInfo, mnt->fs, sizeof(FSModuleInfo));
			
			mnt->fs = modInfo;
			(*vnd)->mount = mnt;

			CharDevice *dev = CharDeviceManager::Device(Inodes->at(inodeIndex)->Major, Inodes->at(inodeIndex)->Minor);
                        (*vnd)->privdata = dev;
			///printk("Major: %i, Minor: %i\n", Inodes->at(inodeIndex)->Major, Inodes->at(inodeIndex)->Minor);
			modInfo->read = dev->read;
			modInfo->write = dev->write;
			modInfo->ioctl = dev->ioctl;
			modInfo->mmap = dev->mmap;
		}else if (S_ISBLK(*ntype)){
                        ///printk("Block device\n");
			FSMount *mnt = new FSMount;
			memcpy(mnt, node->mount, sizeof(FSMount));
			FSModuleInfo *modInfo = new FSModuleInfo;
			memcpy(modInfo, mnt->fs, sizeof(FSModuleInfo));
			
			mnt->fs = modInfo;
			(*vnd)->mount = mnt;

			BlockDevice *dev = BlockDeviceManager::Device(Inodes->at(inodeIndex)->Major, Inodes->at(inodeIndex)->Minor);
                        (*vnd)->privdata = dev;
			///printk("Major: %i, Minor: %i\n", Inodes->at(inodeIndex)->Major, Inodes->at(inodeIndex)->Minor);
			modInfo->read = dev->read;
			modInfo->write = dev->write;
			modInfo->ioctl = dev->ioctl;
			modInfo->mmap = dev->mmap;
		}

		return 0;
	}

	vnd = NULL;

	printk("ENOENT: %s (node addr: %lx)\n", name, (unsigned long) node);

	return -ENOENT;
}

int DevFS::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	return 0;
}

int DevFS::readlink(VNode *node, char *buffer, size_t bufsize)
{
	TmpInode *inode = Inodes->at(node->vnid.id);

	if (!S_ISLNK(inode->Mode)) return -EINVAL;

	if (bufsize > inode->Size) bufsize = inode->Size;

	memcpy(buffer, inode->FileData, bufsize);

	return bufsize;
}

int DevFS::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize, WriteOpFlags flags)
{
	return 0;
}

int DevFS::getdents(VNode *node, dirent *dirp, unsigned int count)
{
    TmpInode *inode = Inodes->at(node->vnid.id);

    if (!S_ISDIR(inode->Mode)) return -ENOTDIR;

    QHash<QString, int>::const_iterator dirIterator = inode->Directory->constBegin();
    unsigned int bufferUsedBytes = 0;
    do{
        rawstrcpy(dirp->d_name, dirIterator.key().toAscii(), sizeof(dirp->d_name), dirIterator.key().length() + 1);
        dirp->d_reclen = sizeof(dirent);
        dirp->d_off = sizeof(dirent); //TODO: ci andrebbe pos
        bufferUsedBytes += dirp->d_reclen;

        dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen);
        ++dirIterator;
    }while ((dirIterator != inode->Directory->constEnd()) && (bufferUsedBytes + sizeof(dirent) < count));

    return bufferUsedBytes;
}

int DevFS::stat(VNode *node, struct stat *buf)
{
	TmpInode *inode = Inodes->at(node->vnid.id);

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

int DevFS::access(VNode *node, int aMode, int uid, int gid)
{
	return 0;
}

int DevFS::chmod(VNode *node, mode_t mode)
{
	return 0;
}

int DevFS::chown(VNode *node, uid_t uid, gid_t gid)
{
	return 0;
}

int DevFS::link(VNode *directory, VNode *oldNode, const char *newName)
{
	return 0;
}

int DevFS::mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev)
{
	TmpInode *tmpInode = new TmpInode;
	tmpInode->Mode = mode;
	tmpInode->Major = dev >> 16; //TODO: Need to be changed to 64 bit
	tmpInode->Minor = dev & 0xFFFF; //TODO: Need to be changed to 64 bit
	tmpInode->Size = 0;

        int id = Inodes->append(tmpInode);

	TmpInode *inode = Inodes->at(1);

	inode->Directory->insert(newName, id);

	return 0;
}

//TODO
int DevFS::truncate(VNode *node, uint64_t length)
{
	return 0;
}

int DevFS::fsync(VNode *node)
{
	return 0;
}

int DevFS::fdatasync(VNode *node)
{
	return 0;
}

int DevFS::unlink(VNode *directory, const char *name)
{
	return -EROFS;
}

int DevFS::rmdir(VNode *directory, const char *name)
{
	return -EROFS;
}

int DevFS::creat(VNode *directory, const char *name, mode_t mode)
{
	return -EROFS;
}

int DevFS::name(VNode *directory, VNode *node, char **name, int *len)
{
	*name = strdup(".");
    *len = 1;

	return 0;
}

int DevFS::statfs(VNode *directory, struct statfs *buf)
{
	return 0;
}

int DevFS::utime(VNode *node, const struct utimbuf *buf)
{
	return 0;
}

int DevFS::symlink(VNode *directory, const char *oldName, const char *newName)
{
     return -EINVAL;
}

int DevFS::rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName)
{
     return -EINVAL;
}

int DevFS::mkdir(VNode *directory, const char *newName, mode_t mode)
{
     return -EINVAL;
}

int DevFS::size(VNode *node, int64_t *size)
{
     return -EINVAL;
}

int DevFS::type(VNode *node, int *type)
{
     return -EINVAL;
}

int DevFS::fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int DevFS::ioctl(VNode *node, int request, long arg)
{
    return -EIOCTLNOTSUPPORTED;
}

void *DevFS::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
     return 0;
}


