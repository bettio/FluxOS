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
 *   Name: bfs.cpp                                                         *
 *   Date: 10/08/2006                                                      *
 ***************************************************************************/

#include <kdef.h>
#include <core/printk.h>
#include <cstdlib.h>
#include <cstring.h>

#include <filesystem/bfs/bfs.h>
#include <filesystem/vnodemanager.h>

struct bfs_privdata
{
	bfs_sblock *sblock;
	BlockDevice *blkdev;
};

using namespace FileSystem;

uint64_t BFS::BlockRunToBlock(bfs_block_run *blockrun, bfs_sblock *sblock)
{
	return (blockrun->allocation_group << sblock->ag_shift) + blockrun->start;
}

int BFS::Init()
{
	FileSystemInfo *info = new FileSystemInfo;
	//TODO: Warning: unchecked malloc
	info->name = "bfs";

	info->mount = Mount;

	FileSystem::VFS::RegisterFileSystem(info);

	return 0;
}

int BFS::Mount(FSMount *fsmount, BlockDevice *blkdev)
{
	//---- Read from disk ------
	void *tmpblk = (void *) malloc(512);
	//TODO: Warning: unchecked malloc

	blkdev->ReadBlock(blkdev, 1, 1, (uint8_t *) tmpblk);
	//------

	bfs_sblock *bfssblock = (bfs_sblock *) tmpblk;

	if (bfssblock->magic1 != SUPER_BLOCK_MAGIC1 | bfssblock->magic2 != SUPER_BLOCK_MAGIC2 | bfssblock->magic3 != SUPER_BLOCK_MAGIC3) while(1);

	FSModuleInfo *info = new FSModuleInfo;
	//TODO: Warning: unchecked malloc
	info->lookup = Lookup;
	info->read = Read;
	info->readlink = ReadLink;

	fsmount->fs = info;

	bfs_privdata *privdata = new bfs_privdata;
	//TODO: Warning: unchecked malloc
	privdata->sblock = bfssblock;
	privdata->blkdev = blkdev;

	fsmount->privdata = (void *) privdata;

	VNode *root;
	VNodeManager::GetVnode(fsmount->mountId, BlockRunToBlock(&bfssblock->root_dir, bfssblock), &root);

	root->mount = fsmount;
	root->vnid.id = BlockRunToBlock(&bfssblock->root_dir, bfssblock);

	fsmount->fsRootVNode = root;

	return 0;
}

bfs_inode *BFS::ReadInode(VNode *node)
{
	bfs_privdata *privdata = (bfs_privdata *) node->mount->privdata;

	//---- Read from disk ------
	void *tmpblk = 0;
	tmpblk = (void *) malloc(5120);
	//TODO: Warning: unchecked malloc
	privdata->blkdev->ReadBlock(privdata->blkdev, node->vnid.id*2, 2, (uint8_t *) tmpblk);
	//----

	bfs_inode *tmpinode = (bfs_inode *) tmpblk;

	if (tmpinode->magic1 != 0x3BBE0AD9){
		printk("I-Node Magick bad! Magic: %x", tmpinode->magic1);
	}

	return tmpinode;
}

int BFS::ReadData(void *inode, VNode *node, char *buffer, unsigned int bufsize)
{
	bfs_inode *tmpinode = (bfs_inode *) inode;
	bfs_privdata *privdata = (bfs_privdata *) node->mount->privdata;

	char *buf = buffer;

	for (int i = 0 /*,sum = 0*/; i < NUM_DIRECT_BLOCKS; /*sum += tmpinode->data.direct[i].len,*/ i++){

		//---- Read from disk ------

		privdata->blkdev->ReadBlock(privdata->blkdev, ((tmpinode->data.direct[i].allocation_group << privdata->sblock->ag_shift) 
		+ tmpinode->data.direct[i].start)*2, tmpinode->data.direct[i].len * 2, (uint8_t *) buf);

		//----

		buf += tmpinode->data.direct[i].len * 2 * 512;
	}

	if (tmpinode->data.indirect.len != 0){
		uint8_t *indirect_data = (uint8_t *) malloc(tmpinode->data.indirect.len*1024);
		//TODO: Warning: unchecked malloc

		privdata->blkdev->ReadBlock(privdata->blkdev, ((tmpinode->data.indirect.allocation_group << privdata->sblock->ag_shift) 
		+ tmpinode->data.indirect.start)*2, tmpinode->data.indirect.len * 2, indirect_data);

		bfs_block_run *indirect_runs = (bfs_block_run *) indirect_data;

		for (unsigned int i = 0 /*,sum = 0*/; i < (tmpinode->data.indirect.len / sizeof(bfs_block_run)); /*sum += tmpinode->data.direct[i].len,*/ i++){

			//---- Read from disk ------

			privdata->blkdev->ReadBlock(privdata->blkdev, ((indirect_runs[i].allocation_group << privdata->sblock->ag_shift) 
			+ indirect_runs[i].start)*2, indirect_runs[i].len * 2, (uint8_t *) buf);

			//----

			buf += indirect_runs[i].len * 2 * 512;
		}
	}

	if (tmpinode->data.double_indirect.len != 0){
		printk("Cannot read files with double indirect!\n");
		while(1);
	}

	//TODO: return proper value
	return 0;
}

int BFS::Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	bfs_inode *inode = (bfs_inode *) ReadInode(node);

	return ReadData(inode, node, buffer, bufsize);
}

uint16_t *BFS::GetLengthIndex(bfs_btree_node *node)
{
	unsigned int round = 0;

	//8 is an empirical value, the official documentation says 4
	if(((sizeof(bfs_btree_node) + node->all_key_length) % 8) != 0) {
		round = 8 - ((sizeof(bfs_btree_node) + node->all_key_length) % 8);
	}

	return (uint16_t *) ((unsigned long) node + sizeof(bfs_btree_node) + node->all_key_length + round);
}

uint64_t *BFS::GetValueArray(bfs_btree_node *node)
{
	return (uint64_t *) ((unsigned long) GetLengthIndex(node) + sizeof(uint16_t) * node->all_key_count);
}

char *BFS::GetKeydata(bfs_btree_node *node)
{
	return (char *) node + sizeof(bfs_btree_node);
}


uint64_t BFS::GetValue(const char *key, bfs_btree_root *treeroot)
{
	bfs_btree_node *rootnode = (bfs_btree_node *) ((unsigned long) treeroot +  (unsigned long) treeroot->root_node_pointer);

	char *tmpstr = (char *) malloc(256);
	//TODO: Warning: unchecked malloc

	bfs_btree_node *tmpnode;

	if (rootnode->overflow != 0xFFFFFFFFFFFFFFFFLL){
		tmpnode = (bfs_btree_node *) ((unsigned long) treeroot + (unsigned long)  rootnode->overflow);
	}else{
		tmpnode = rootnode;
	}

	uint64_t *tmpvalue = GetValueArray(tmpnode);
	uint16_t *tmplength = GetLengthIndex(tmpnode);

	while(true){
		#ifdef BFS_DEBUG
			printk("First length %i\n", (unsigned int) tmplength[0]);
		#endif

		for (int i = 0; i < tmpnode->all_key_count; i++){
			if (i == 0){
				rawstrcpy(tmpstr, GetKeydata(tmpnode), 256, tmplength[0]);
			}else{
				rawstrcpy(tmpstr, GetKeydata(tmpnode) + tmplength[i - 1], 256, tmplength[i] - tmplength[i - 1]);
			}

			#ifdef BFS_DEBUG
				printk("Found %s\n", tmpstr);
			#endif

			if (!strcmp(tmpstr, key)){
				#ifdef BFS_DEBUG
					printk("Found! The key is %x\n", (unsigned int) tmpvalue[i]);
				#endif

				return tmpvalue[i];
			}
		}

		if (tmpnode->left != 0xFFFFFFFFFFFFFFFFLL){
			tmpnode = (bfs_btree_node *) ((unsigned long) treeroot + (unsigned long) tmpnode->left);
			tmpvalue = GetValueArray(tmpnode);
			tmplength = GetLengthIndex(tmpnode);
			continue;
		}else{
			
			tmpnode = (bfs_btree_node *) ((unsigned long) treeroot + (unsigned long) tmpnode->right);
			tmpvalue = GetValueArray(tmpnode);
			tmplength = GetLengthIndex(tmpnode);
			continue;
		}
	}

}

int BFS::ReadLink(VNode *node, char *buf, size_t bufsize)
{
	bfs_inode *inode = (bfs_inode *) ReadInode(node);

	if (S_ISLNK(inode->mode) && !(inode->flags & BEFS_LONG_SYMLINK)){
		char *shortlink = (char *) &inode->data;
		strlcpy(buf, shortlink, bufsize);
	}else if (S_ISLNK(inode->mode) && (inode->flags & BEFS_LONG_SYMLINK)){
		ReadData((void *) inode, node, buf, bufsize);
	}else{
		return 1;
	}

	return 0;
}

int BFS::Lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
	#ifdef BFS_DEBUG
		printk("Lookup %s\n", name);
	#endif

	//Read the inode
	bfs_inode *inode = (bfs_inode *) ReadInode(node);

	//Read inode data
	char *tmpdata = (char *) malloc(10240);
	//TODO: Warning: unchecked malloc
	ReadData((void *) inode, node, tmpdata, 10240);

	//Check the header
	bfs_btree_root *tree_root = (bfs_btree_root *) tmpdata;
	if (tree_root->magic != BEFS_BTREE_MAGIC) printk("Corrupted B+Tree! Magic:%x\n", tree_root->magic);

	//Get the VNode
	//TODO: ENOENT should be managed
	VNodeManager::GetVnode(node->mount->mountId, GetValue(name, tree_root), vnd);

	(*vnd)->mount = node->mount;

	*ntype = ReadInode(*vnd)->mode;

	return 0;
}
