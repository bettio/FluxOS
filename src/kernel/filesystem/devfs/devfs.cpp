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
#include <cstdlib.h>
#include <cstring.h>
#include <arch.h>
#include <lib/koof/vector.h>
#include <lib/koof/keymap.h>


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
	KeyMap<int> *Directory;
};

using namespace FileSystem;

Vector<TmpInode *> Inodes;

int DevFS::Init()
{
	Inodes = Vector<TmpInode *>();
	TmpInode *tmpInode = new TmpInode;
	tmpInode->Mode = S_IFDIR;
	tmpInode->Directory = new KeyMap<int>();
	tmpInode->Directory->Add(".", 1);
	tmpInode->Directory->Add("..", 1);
	Inodes.Add(0);
	Inodes.Add(tmpInode);

	return 0;
}

int DevFS::RegisterAsFileSystem()
{
	FileSystemInfo *info = new FileSystemInfo;
	if (info == NULL) return -ENOMEM;
	info->name = "devfs";

	info->mount = Mount;

	FileSystem::VFS::RegisterFileSystem(info);
}

int DevFS::Mount(FSMount *fsmount, BlockDevice *)
{
	FSModuleInfo *info = new FSModuleInfo;
	if (info == NULL) return -ENOMEM;
	info->lookup = Lookup;
	info->read = Read;
	info->readlink = Readlink;
	info->write = Write;
	info->getdents = GetDEnts;
	info->stat = Stat;
	info->name = Name;
	info->access = Access;
	info->chmod = Chmod;
	info->chown = Chown;
	info->link = Link;
	info->mknod = Mknod;
	info->truncate = Truncate;
	info->fsync = FSync;
	info->fdatasync = FDataSync;
	info->rmdir = Rmdir;
	info->creat = Creat;

	fsmount->fs = info;

	VNode *root; 
	VNodeManager::GetVnode(fsmount->mountId, 1, &root);

	root->mount = fsmount;
	root->vnid.id = 1;
	root->vnid.mountId = fsmount->mountId;

	fsmount->fsRootVNode = root;

	//TODO: Ritornare un valore adeguato
	return 0;
}

int DevFS::Lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
	TmpInode *inode = Inodes[node->vnid.id];
	
	if (!S_ISDIR(inode->Mode)) return -ENOTDIR;

	int inodeIndex = (*inode->Directory)[name];
	
	if (inodeIndex){
		VNodeManager::GetVnode(node->mount->mountId, inodeIndex, vnd);

		(*vnd)->mount = node->mount;
		*ntype = Inodes[inodeIndex]->Mode;

		if (S_ISCHR(*ntype)){
			///printk("Char device\n");
			FSMount *mnt = new FSMount;
			memcpy(mnt, node->mount, sizeof(FSMount));
			FSModuleInfo *modInfo = new FSModuleInfo;
			memcpy(modInfo, mnt->fs, sizeof(FSModuleInfo));
			
			mnt->fs = modInfo;
			(*vnd)->mount = mnt;

			CharDevice *dev = CharDeviceManager::Device(Inodes[inodeIndex]->Major, Inodes[inodeIndex]->Minor);
			///printk("Major: %i, Minor: %i\n", Inodes[inodeIndex]->Major, Inodes[inodeIndex]->Minor);
			modInfo->read = dev->read;
			modInfo->write = dev->write;
			modInfo->ioctl = dev->ioctl;
		}else if (S_ISCHR(*ntype)){
			///printk("Block device\n");
			FSMount *mnt = new FSMount;
			memcpy(mnt, node->mount, sizeof(FSMount));
			FSModuleInfo *modInfo = new FSModuleInfo;
			memcpy(modInfo, mnt->fs, sizeof(FSModuleInfo));
			
			mnt->fs = modInfo;
			(*vnd)->mount = mnt;

			BlockDevice *dev = BlockDeviceManager::Device(Inodes[inodeIndex]->Major, Inodes[inodeIndex]->Minor);
			///printk("Major: %i, Minor: %i\n", Inodes[inodeIndex]->Major, Inodes[inodeIndex]->Minor);
			modInfo->read = dev->read;
			modInfo->write = dev->write;
			modInfo->ioctl = dev->ioctl;
		}

		return 0;
	}

	vnd = NULL;

	printk("ENOENT: %s (node addr: %x)\n", name, node);

	return -ENOENT;
}

int DevFS::Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	return 0;
}

int DevFS::Readlink(VNode *node, char *buffer, size_t bufsize)
{
	TmpInode *inode = Inodes[node->vnid.id];

	if (!S_ISLNK(inode->Mode)) return -EINVAL;

	if (bufsize > inode->Size) bufsize = inode->Size;

	memcpy(buffer, inode->FileData, bufsize);

	return bufsize;
}

int DevFS::Write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
	return 0;
}

int DevFS::GetDEnts(VNode *node, dirent *dirp, unsigned int count)
{
	TmpInode *inode = Inodes[node->vnid.id];
	
	if (!S_ISDIR(inode->Mode)) return -ENOTDIR;

	KeyMap<int> *dir = inode->Directory;

	unsigned int bufferUsedBytes = 0;
	int i = 0;

	do{
		rawstrcpy(dirp->d_name, (*dir)[i], sizeof(dirp->d_name), strlen((*dir)[i]) + 1);
		dirp->d_reclen = sizeof(dirent);
		dirp->d_off = sizeof(dirent); //TODO: ci andrebbe pos
		bufferUsedBytes += dirp->d_reclen;

		dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen);
		i++;
	}while ((i < dir->Size()) && (bufferUsedBytes + sizeof(dirent) < count));

	return bufferUsedBytes;
}

int DevFS::Stat(VNode *node, struct stat *buf)
{
	TmpInode *inode = Inodes[node->vnid.id];

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

int DevFS::Access(VNode *node, int aMode, int uid, int gid)
{
	return 0;
}

int DevFS::Chmod(VNode *node, mode_t mode)
{
	return 0;
}

int DevFS::Chown(VNode *node, uid_t uid, gid_t gid)
{
	return 0;
}

int DevFS::Link(VNode *directory, VNode *oldNode, const char *newName)
{
	return 0;
}

int DevFS::Mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev)
{
	TmpInode *tmpInode = new TmpInode;
	tmpInode->Mode = mode;
	tmpInode->Major = dev >> 16; //TODO: Need to be changed to 64 bit
	tmpInode->Minor = dev & 0xFFFF; //TODO: Need to be changed to 64 bit
	tmpInode->Size = 0;

	int id = Inodes.Add(tmpInode);

	TmpInode *inode = Inodes[1];

	inode->Directory->Add(newName, id);

	return 0;
}

//TODO
int DevFS::Truncate(VNode *node, uint64_t length)
{
	return 0;
}

int DevFS::FSync(VNode *node)
{
	return 0;
}

int DevFS::FDataSync(VNode *node)
{
	return 0;
}

int DevFS::Unlink(VNode *directory, const char *name)
{
	return -EROFS;
}

int DevFS::Rmdir(VNode *directory, const char *name)
{
	return -EROFS;
}

int DevFS::Creat(VNode *directory, const char *name, mode_t mode)
{
	return -EROFS;
}

int DevFS::Name(VNode *directory, VNode *node, char **name, int *len)
{
	*name = strdup(".");
    *len = 1;

	return 0;
}

int DevFS::StatFS(VNode *directory, struct statfs *buf)
{
	return 0;
}

int DevFS::Utime(VNode *node, const struct utimbuf *buf)
{
	return 0;
}
