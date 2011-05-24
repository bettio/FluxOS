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
 *   Name: procfs.h                                                         *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#ifndef _FILESYSTEM_PROCFS_PROCFS_H_
#define _FILESYSTEM_PROCFS_PROCFS_H_

#include <filesystem/vfs.h>

namespace FileSystem
{
	class ProcFS
	{
		public:
			static int Init();
			static int Mount(FSMount *fsmount, BlockDevice *blkdev);
			static int Lookup(VNode *node, const char *name,VNode **vnd, unsigned int *ntype);
			static int Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
			static int GetDEnts(VNode *node, dirent *dirp, unsigned int count);
			static int Stat(VNode *node, struct stat *buf);
	};
}

#endif
