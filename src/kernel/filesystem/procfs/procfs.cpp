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

FSCALLS(ProcFS::calls);

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
    fsmount->fs = &calls;

	VNode *root;
	VNodeManager::GetVnode(fsmount->mountId, 1, &root);
	root->mount = fsmount;	
	root->vnid.id = 1;

	fsmount->fsRootVNode = root;

	//TODO: return a proper value
	return 0;
}

int ProcFS::umount(VNode *root)
{
     return -EINVAL;
}

int ProcFS::socketcall(VNode *node, int call, unsigned long *args)
{
     return -EINVAL;
}

int ProcFS::openfd(VNode *node, FileDescriptor *fdesc)
{
     return -EINVAL;
}

int ProcFS::closefd(VNode *node, FileDescriptor *fdesc)
{
     return -EINVAL;
}

int ProcFS::dupfd(VNode *node, FileDescriptor *fdesc)
{
     return -EINVAL;
}

int ProcFS::lookup(VNode *node, const char *name,VNode **vnd, unsigned int *ntype)
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

int ProcFS::closevnode(VNode *node)
{
     return 0;
}

int ProcFS::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	if (node->vnid.id == 2){
		if (pos < sizeof("FluxOS Kernel 0.1\n") - 1){
		    strncpy(buffer, "FluxOS Kernel 0.1\n" + pos, bufsize);
		    return bufsize;
		}else{
		    return 0;
		}
	}

	return 0;
}

int ProcFS::readlink(VNode *node, char *buffer, size_t bufsize)
{
     return -EINVAL;
}

int ProcFS::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
     return -EINVAL;
}

int ProcFS::getdents(VNode *node, dirent *dirp, unsigned int count)
{
    int size = 0;

	strcpy(dirp->d_name, ".");
	dirp->d_reclen = sizeof(dirent);
	dirp->d_off = 268;
    size += dirp->d_reclen;

	dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen);

	strcpy(dirp->d_name, "..");
	dirp->d_reclen = sizeof(dirent);
	dirp->d_off = 268;
    size += dirp->d_reclen;

	dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen);

	strcpy(dirp->d_name, "version");
	dirp->d_reclen = sizeof(dirent);
	dirp->d_off = 268;
    size += dirp->d_reclen;

	return size;

}

int ProcFS::stat(VNode *node, struct stat *buf)
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

int ProcFS::access(VNode *node, int aMode, int uid, int gid)
{
     return -EINVAL;
}

int ProcFS::name(VNode *directory, VNode *node, char **name, int *len)
{
     return -EINVAL;
}


int ProcFS::chmod(VNode *node, mode_t mode)
{
     return -EINVAL;
}

int ProcFS::chown(VNode *node, uid_t uid, gid_t gid)
{
     return -EINVAL;
}

int ProcFS::link(VNode *directory, VNode *oldNode, const char *newName)
{
     return -EINVAL;
}

int ProcFS::symlink(VNode *directory, const char *oldName, const char *newName)
{
     return -EINVAL;
}

int ProcFS::rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName)
{
     return -EINVAL;
}

int ProcFS::mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev)
{
     return -EINVAL;
}

int ProcFS::mkdir(VNode *directory, const char *newName, mode_t mode)
{
     return -EINVAL;
}

int ProcFS::truncate(VNode *node, uint64_t length)
{
     return -EINVAL;
}

int ProcFS::fsync(VNode *node)
{
     return -EINVAL;
}

int ProcFS::fdatasync(VNode *node)
{
     return -EINVAL;
}

int ProcFS::unlink(VNode *directory, const char *name)
{
     return -EINVAL;
}

int ProcFS::rmdir(VNode *directory, const char *name)
{
     return -EINVAL;
}

int ProcFS::creat(VNode *directory, const char *name, mode_t mode)
{
     return -EINVAL;
}

int ProcFS::statfs(VNode *directory, struct statfs *buf)
{
     return -EINVAL;
}

int ProcFS::size(VNode *node, int64_t *size)
{
     return -EINVAL;
}

int ProcFS::type(VNode *node, int *type)
{
     return -EINVAL;
}

int ProcFS::utime(VNode *node, const struct utimbuf *buf)
{
     return -EINVAL;
}

int ProcFS::fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int ProcFS::ioctl(VNode *node, int request, long arg)
{
    return -EIOCTLNOTSUPPORTED;
}

void *ProcFS::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
     return 0;
}


