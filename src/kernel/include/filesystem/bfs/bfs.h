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
 *   Name: bfs.h                                                         *
 *   Date: 10/08/2006                                                      *
 ***************************************************************************/

#ifndef _FILESYSTEM_BFS_BFS_H_
#define _FILESYSTEM_BFS_BFS_H_

#include <filesystem/vfs.h>
#include <drivers/blockdevicemanager.h>

#include "bfs_structs.h"

namespace FileSystem
{
	class BFS
	{
		public:
			static int Init();
			static int Mount(FSMount *fsmount, BlockDevice *blkdev);
			static int Lookup(VNode *node, const char *name,VNode **vnd, unsigned int *ntype);
			static int ReadData(void *inode, VNode *node, char *buffer, unsigned int bufsize);
			static bfs_inode *ReadInode(VNode *node);
			static int Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
			static uint16_t *GetLengthIndex(bfs_btree_node *node);
			static uint64_t *GetValueArray(bfs_btree_node *node);
			static char *GetKeydata(bfs_btree_node *node);
			static uint64_t GetValue(const char *key, bfs_btree_root *treeroot);
			static uint64_t BlockRunToBlock(bfs_block_run *blockrun, bfs_sblock *sblock);
			static int ChOwn(VNode *node, unsigned int owner, unsigned int group);
			static int ChMod(VNode *node, unsigned int mode);
			static int ReadLink(VNode *node, char *buf, size_t bufsize);
	};
}

#endif
