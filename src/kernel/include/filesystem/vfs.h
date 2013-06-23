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
 *   Name: vfs.h                                                           *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/


#ifndef _FILESYSTEM_VFS_H_
#define _FILESYSTEM_VFS_H_

#include <filesystem/vnode.h>
#include <filesystem/fsmount.h>
#include <filesystem/fsmoduleinfo.h>
#include <filesystem/dirent.h>
#include <filesystem/filesysteminfo.h>
#include <filesystem/stat.h>
#include <filesystem/stat64.h>
#include <errors.h>

#include <stdint.h>
#include <string.h>
#ifdef USE_GLIBC
#include <stdio.h>
#include <stdlib.h>
#endif

#include <QHash>

#include <kdef.h>

#define FS_CALL(vnode, op) (vnode->mount->fs->op)
#define FS_MOUNT_CALL(mount, op) (mount->fs->op)

#define RESERVED_MOUNT_IDS 0xFFFFFFF0
#define PIPE_MOUNTID 0xFFFFFFF1
#define SOCKET_MOUNTID 0xFFFFFFF7

class QString;
            
namespace FileSystem
{
	class VFS
	{
        	public:
			static void Init();
			static int RegisterFileSystem(FileSystemInfo *fsinfo);
			static int UnregisterFileSystem(const char *name);
			static int Mount(const char *source, const char *mountpoint, const char *fstype, unsigned int flags, const void *data);
			static int Umount(const char *mountpoint);
			static MUST_CHECK int RelativePathToVnode(VNode *start, const char *path, VNode **node, bool traverse_leaf_link = true, int count = 0);
			static MUST_CHECK int GetDirPathFromVnode(VNode *node, char **pathFromVnode);

		private:
			static VNode *RootNode;
			static unsigned int FirstFreeMntID;
			static QHash<QString, FileSystemInfo *> FileSystems;
	};
}

#endif
