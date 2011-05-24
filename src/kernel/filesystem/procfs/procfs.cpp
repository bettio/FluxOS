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
 *   Name: procfs.cpp                                                      *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#include <kdef.h>
#include <core/printk.h>
#include <cstdlib.h>
#include <cstring.h>

#include <filesystem/vfs.h>
#include <filesystem/procfs/procfs.h>
#include <filesystem/vnodemanager.h>

using namespace FileSystem;

int ProcFS::Init()
{
	FileSystemInfo *info = new FileSystemInfo;
	//TODO: Warning: unchecked malloc
	info->name = "procfs";

	info->mount = Mount;

	FileSystem::VFS::RegisterFileSystem(info);

	return 0;
}

int ProcFS::Mount(FSMount *fsmount, BlockDevice *blkdev)
{
	FSModuleInfo *info = new FSModuleInfo;
	//TODO: Warning: unchecked malloc
	info->lookup = Lookup;
	info->read = Read;
	info->getdents = GetDEnts;
	info->stat = Stat;

	fsmount->fs = info;

	VNode *root;
	VNodeManager::GetVnode(fsmount->mountId, 1, &root);
	root->mount = fsmount;	
	root->vnid.id = 1;

	fsmount->fsRootVNode = root;

	//TODO: return a proper value
	return 0;
}

int ProcFS::Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	if (node->vnid.id == 2){
		if (pos < sizeof("FluxOS 0.1\n")){
		    strncpy(buffer, "FluxOS 0.1\n" + pos, bufsize);
		    return bufsize;
		}else{
		    return 0;
		}
	}

	return 0;
}

//TODO: Propongo un refactory del codice
int ProcFS::Lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
	if ((node->vnid.id == 1) && (!strcmp("version", name))){
		VNodeManager::GetVnode(node->mount->mountId, 2, vnd);

		VNode *tmp = *vnd;
		tmp->mount = node->mount;

		*vnd = tmp;

		return 0;

	}else if ((node->vnid.id == 1) && (!strcmp(".", name))){
		VNodeManager::GetVnode(node->mount->mountId, 1, vnd);

		VNode *tmp = *vnd;
		tmp->mount = node->mount;

		*vnd = tmp;

		return 0;

	}else if ((node->vnid.id == 1) && (!strcmp("..", name))){
		VNodeManager::GetVnode(node->mount->mountId, 1, vnd);

		VNode *tmp = *vnd;
		tmp->mount = node->mount;

		*vnd = tmp;

		return 0;
	}

	*vnd = 0;

	return -ENOENT;
}

int ProcFS::GetDEnts(VNode *node, dirent *dirp, unsigned int count)
{
	strcpy(dirp->d_name, ".");
	dirp->d_reclen = sizeof(dirent);
	dirp->d_off = 268;

	dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen);

	strcpy(dirp->d_name, "..");
	dirp->d_reclen = sizeof(dirent);
	dirp->d_off = 268;

	dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen);

	strcpy(dirp->d_name, "version");
	dirp->d_reclen = sizeof(dirent);
	dirp->d_off = 268;


	return 0;
}

int ProcFS::Stat(VNode *node, struct stat *buf)
{
	if (node->vnid.id == 1){
		buf->st_dev = 0;
		buf->st_ino = 1;
		buf->st_mode = S_IFDIR | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
		buf->st_nlink = 1;
		buf->st_uid = 0;
		buf->st_gid = 0;
		buf->st_rdev = 0;
		buf->st_size = 0;
		buf->st_blksize = 0;
		buf->st_blocks = 0;
		buf->st_atime = 0;//time(0);
		buf->st_mtime = 0;//time(0);
		buf->st_ctime = 0;//time(0);
	}else if (node->vnid.id == 2){
		buf->st_dev = 0;
		buf->st_ino = 1;
		buf->st_mode = S_IRUSR | S_IRGRP | S_IROTH;
		buf->st_nlink = 1;
		buf->st_uid = 0;
		buf->st_gid = 0;
		buf->st_rdev = 0;
		buf->st_size = 0;
		buf->st_blksize = 0;
		buf->st_blocks = 0;
		buf->st_atime = 0;//time(0);
		buf->st_mtime = 0;//time(0);
		buf->st_ctime = 0;//time(0);
	}

	return 0;
}
