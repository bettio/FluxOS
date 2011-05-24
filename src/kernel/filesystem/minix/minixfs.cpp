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
 *   Name: minixfs.h                                                       *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#include <kdef.h>
#include <core/printk.h>
#include <cstdlib.h>
#include <cstring.h>
#include <arch.h>

#include <filesystem/vfs.h>
#include <filesystem/minix/minixfs.h>
#include <filesystem/vnodemanager.h>
#include <drivers/blockdevicemanager.h>

#define MINIX_MAGIC	0x137F
#define MINIX2_MAGIC	0x2468
#define MINIX_MAGIC_30	0x138F
#define MINIX2_MAGIC_30	0x2478
#define BLOCK_SIZE 512
#define LOG2 1

//#define BITS_BIG_ENDIAN 0

#if BITS_BIG_ENDIAN
	#define INT16_ENDIAN_SWAP(int16) int16 = ((int16 >> 8) | (int16 << 8))
	#define INT32_ENDIAN_SWAP(int32) int32 = (((int32 & 0xFF000000) >> 8) | ((int32 & 0x00FF0000) << 8) | ((int32 & 0x0000FF00) >> 8) | ((int32 & 0x000000FF) << 8))
#else
	#define INT16_ENDIAN_SWAP(int16)
	#define INT32_ENDIAN_SWAP(int16)
#endif

using namespace FileSystem;

struct minix_sblock
{
  uint16_t inode_cnt;
  uint16_t zone_cnt;
  uint16_t inode_bmap_size;
  uint16_t zone_bmap_size;
  uint16_t first_data_zone;
  uint16_t log2_zone_size;
  uint32_t max_file_size;
  uint16_t magic;
} __attribute__ ((packed));

struct minix_inode
{
  uint16_t mode;
  uint16_t uid;
  uint32_t size;
  uint32_t ctime;
  uint8_t gid;
  uint8_t nlinks;
  uint16_t dir_zones[7];
  uint16_t indir_zone;
  uint16_t double_indir_zone;
} __attribute__ ((packed));

struct minix2_inode
{
  uint16_t mode;
  uint16_t nlinks;
  uint16_t uid;
  uint16_t gid;
  uint32_t size;
  uint32_t atime;
  uint32_t mtime;
  uint32_t ctime;
  uint32_t dir_zones[7];
  uint32_t indir_zone;
  uint32_t double_indir_zone;
  uint32_t unused;
} __attribute__ ((packed));

struct minix_privdata
{
	minix_sblock *sblock;
	BlockDevice *blkdev;
};


struct VNode_priv_data
{
	unsigned int inode;
};

int MinixFS::Init()
{
	FileSystemInfo *info = new FileSystemInfo;
	//TODO: Warning: unchecked malloc
	info->name = "minixfs";

	info->mount = Mount;

	FileSystem::VFS::RegisterFileSystem(info);

	return 0;
}

int MinixFS::Mount(FSMount *fsmount, BlockDevice *blkdev)
{
	//------
	void *tmpblk = (void *) malloc(512);
	//TODO: Warning: unchecked malloc

	blkdev->ReadBlock(blkdev, 2, 1, (uint8_t *) tmpblk);

	minix_sblock *sblock = (minix_sblock *) tmpblk;

	INT16_ENDIAN_SWAP(sblock->inode_cnt);
	INT16_ENDIAN_SWAP(sblock->zone_cnt);
	INT16_ENDIAN_SWAP(sblock->inode_bmap_size);
	INT16_ENDIAN_SWAP(sblock->zone_bmap_size);
	INT16_ENDIAN_SWAP(sblock->first_data_zone);
	INT16_ENDIAN_SWAP(sblock->log2_zone_size);
	INT32_ENDIAN_SWAP(sblock->max_file_size);
	INT16_ENDIAN_SWAP(sblock->magic);
	
	//------

	switch ( sblock->magic ){
		case 0x137F:
			printk("minixfs v1 with 14 chars file names.\n");

			break;

		case 0x138F:
			printk("minixfs v1 with 30 chars file names.\n");

			break;

		case 0x2468:
			printk("minixfs v2 with 14 chars file names.\n");

			break;

		case 0x2478:
			printk("minixfs v2 with 30 chars file names.\n");

			break;

		default:
			printk("unknow filesystem type: magic seems to be %x.\n", sblock->magic);
			while(1);
	}

	FSModuleInfo *info = new FSModuleInfo;
	//TODO: Warning: unchecked malloc
	info->lookup = Lookup;
	info->read = Read;
	info->readlink = Readlink;
	info->getdents = GetDEnts;
	info->stat = Stat;

	fsmount->fs = info;

	minix_privdata *privdata = new minix_privdata;
	//TODO: Warning: unchecked malloc
	privdata->sblock = sblock;
	privdata->blkdev = blkdev;

	fsmount->privdata = (void *) privdata;

	VNode *root;
	VNodeManager::GetVnode(fsmount->mountId, 1, &root);

	root->mount = fsmount;
	root->vnid.id = 1;
	root->vnid.mountId = fsmount->mountId;

	fsmount->fsRootVNode = root;

	return 0;
}

void *MinixFS::ReadInode(VNode *node)
{
	minix_privdata *privdata = (minix_privdata *) node->mount->privdata;

	//Read from disk
	static void *tmpblk = 0;
	if (tmpblk == 0){
		tmpblk = (void *) malloc(5120);
		//TODO: Warning: unchecked malloc
		privdata->blkdev->ReadBlock(privdata->blkdev, (privdata->sblock->inode_bmap_size * 2 + privdata->sblock->zone_bmap_size *2 + 4), 5, (uint8_t *) tmpblk);
	}

	if (node->privdata == 0){
		minix_inode *inodev1 = (minix_inode *) ((unsigned long) ((node->vnid.id - 1) * sizeof(minix_inode)) + (unsigned long) tmpblk);

		INT16_ENDIAN_SWAP(inodev1->mode);
		INT16_ENDIAN_SWAP(inodev1->uid);
		INT32_ENDIAN_SWAP(inodev1->size);
		INT32_ENDIAN_SWAP(inodev1->ctime);
		//Here: uint8_t gid
		//Here: uint8_t nlinks
		INT16_ENDIAN_SWAP(inodev1->dir_zones[0]);
		INT16_ENDIAN_SWAP(inodev1->dir_zones[1]);
		INT16_ENDIAN_SWAP(inodev1->dir_zones[2]);
		INT16_ENDIAN_SWAP(inodev1->dir_zones[3]);
		INT16_ENDIAN_SWAP(inodev1->dir_zones[4]);
		INT16_ENDIAN_SWAP(inodev1->dir_zones[5]);
		INT16_ENDIAN_SWAP(inodev1->dir_zones[6]);
		INT16_ENDIAN_SWAP(inodev1->indir_zone);
		INT16_ENDIAN_SWAP(inodev1->double_indir_zone);

		node->privdata = (void *) inodev1;
	}

	return node->privdata;
}

//TODO: read from any position of any size has to be implemented
int MinixFS::ReadData(void *inode, VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	char *buf = buffer;

	minix_inode *inode2 = (minix_inode *) inode;
	
	minix_privdata *privdata = (minix_privdata *) node->mount->privdata;

	for (unsigned int i = 0; i < 7; i++){
		if (inode2->dir_zones[i] == 0) break;

		privdata->blkdev->ReadBlock(privdata->blkdev, (inode2->dir_zones[i] << 1), 2, (uint8_t *) buf);

		buf += 1024;
	}

	if (inode2->indir_zone != 0){
		uint16_t *indirbuf = (uint16_t *) malloc(1024);
		//TODO: Warning: unchecked malloc 

		privdata->blkdev->ReadBlock(privdata->blkdev, (inode2->indir_zone << 1), 2, (uint8_t *) indirbuf);

		for (unsigned int i = 0; i < 64; i++){
			if (indirbuf[i] == 0) break;

			privdata->blkdev->ReadBlock(privdata->blkdev,(indirbuf[i] << 1), 2, (uint8_t *) buf);

			buf += 1024;
		}
	}

	//TODO: Double indir buf support.

	//TODO: Return a proper value
	return 0;
}

int MinixFS::ReadDirectoryEntry(char *buffer, unsigned int entrynum, uint16_t **entryinode, char **entryname)
{
	*entryinode = (uint16_t *) ((unsigned long) buffer + 32*entrynum);
	*entryname = (char *) ((unsigned long) buffer + 2 + 32*entrynum);

	INT16_ENDIAN_SWAP(**entryinode);

	//TODO: I should return a proper value
	return 0;
}

int MinixFS::Readlink(VNode *node, char *buffer, size_t bufsize)
{
	minix_inode *inode = (minix_inode *) ReadInode(node);

	return ReadData(inode, node, 0, buffer, bufsize);

	return 0;
}

int MinixFS::Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	minix_inode *inode = (minix_inode *) ReadInode(node);

	return ReadData(inode, node, 0, buffer, bufsize);
}

int MinixFS::Lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
	minix_privdata *privdata = (minix_privdata *) node->mount->privdata;

	#ifdef MINIX_DEBUG
		printk("Lookup %s\n", name);
	#endif

	minix_inode *inode = (minix_inode *) ReadInode(node);

	void *tmpblk2 = (void *) malloc(5120*2);
	//TODO: Warning: unchecked malloc
	ReadData(inode, node, 0, (char *) tmpblk2, 5120*2);

	for (unsigned int i = 0; i < inode->size / 32; i++){

		uint16_t *entinode;
		char *entryname;

		ReadDirectoryEntry((char *) tmpblk2, i, &entinode, &entryname);

		#ifdef MINIXFS_DEBUG
			printk("Found: %s\n", entryname);
		#endif

		if (!strcmp(name, entryname)){
			if (*entinode <= privdata->sblock->inode_cnt){
				VNodeManager::GetVnode(node->mount->mountId, *entinode, vnd);

				(*vnd)->mount = node->mount;

				*ntype = ((minix_inode *) ReadInode(*vnd))->mode;

				return 0;
			}else{
				#ifdef MINIXFS_DEBUG
					printk("Error: the file doesn't exist!");
				#endif
			}
		}
	}

	*vnd = 0;

	return -ENOENT;
}

int MinixFS::GetDEnts(VNode *node, dirent *dirp, unsigned int count)
{
	minix_inode *inode = (minix_inode *) ReadInode(node);

	void *tmpblk2 = (void *) malloc(5120*2);
	//TODO: Warning: unchecked malloc
	ReadData(inode, node, 0, (char *) tmpblk2, 5120*2);

	for (unsigned int i = 0; i < inode->size / 32; i++){

		uint16_t *entinode;
		char *entryname;

		ReadDirectoryEntry((char *) tmpblk2, i, &entinode, &entryname);

		#ifdef MINIXFS_DEBUG
			printk("Entry %i: %s\n", i, entryname);
		#endif

		strcpy(dirp->d_name, entryname);
		dirp->d_reclen = sizeof(dirent);
		dirp->d_off = sizeof(dirent);

		dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen);
	}

	return 0;
}

int MinixFS::Stat(VNode *node, struct stat *buf)
{
	minix_inode *inode = (minix_inode *) ReadInode(node);

	buf->st_dev = 0;
	buf->st_ino = node->vnid.id;
	buf->st_mode = inode->mode;
	buf->st_nlink = inode->nlinks;
	buf->st_uid = inode->uid;
	buf->st_gid = inode->gid;
	buf->st_rdev = 0;
	buf->st_size = inode->size;
	buf->st_blksize = 0;
	buf->st_blocks = 0;
	buf->st_atime = inode->ctime;
	buf->st_mtime = inode->ctime;
	buf->st_ctime = inode->ctime;

	return 0;
}
