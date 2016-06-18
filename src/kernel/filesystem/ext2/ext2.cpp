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
 *   Name: ext2.cpp                                                        *
 *   Date: 20/05/2007                                                      *
 ***************************************************************************/

#include <cstdlib.h>
#include <cstring.h>

#include <filesystem/vfs.h>
#include <filesystem/ext2/ext2.h>
#include <filesystem/vnodemanager.h>
#include <filesystem/utimbuf.h>
#include <drivers/blockdevicemanager.h>
#include <core/printk.h>

#define ENABLE_DEBUG_MSG 0

#include <debugmacros.h>

//#define EXT2_DEBUG

struct ext2_privdata
{
	ext2_super_block *sblock;
	BlockDevice *blkdev;

	unsigned int BlockSize;
	unsigned int DiskBlockLog;
	unsigned int IndirectBlockEntries;
	unsigned int DiskBlocksPerFSBlock;

	unsigned int DirectBlocksAreaSize;
	unsigned int IndirectBlocksAreaSize;
	unsigned int DoubleIndirectBlocksAreaSize;
	unsigned int DirectIndirectBlocksAreaSize;
	unsigned int DirIndirDIndirBlocksAreaSize;
};

using namespace FileSystem;

int Ext2::Init()
{
	FileSystemInfo *info = new FileSystemInfo;
	if (info == NULL) return -ENOMEM;
	info->name = "ext2";

	info->mount = Mount;

	FileSystem::VFS::RegisterFileSystem(info);

	return 0;
}

int Ext2::Mount(FSMount *fsmount, BlockDevice *blkdev)
{
	//------
	void *tmpblk = (void *) malloc(512);
	if (tmpblk == NULL) return -ENOMEM;

	blkdev->ReadBlock(blkdev, 2, 1, (uint8_t *) tmpblk);

	ext2_super_block *sblock = (ext2_super_block *) tmpblk;

	//------

	if (sblock->s_magic != EXT2_MAGIC){
		printk("unknow filesystem type: magic seems to be %x.\n", sblock->s_magic);

		free(sblock);

		return -EINVAL;
	}

	FSModuleInfo *info = new FSModuleInfo;
	if (info == NULL){
		free(sblock);

		return -ENOMEM;
	}
    info->umount = Umount;
    info->openfd = OpenFD;
    info->dupfd = DupFD;
    info->closefd = CloseFD;
    info->lookup = Lookup;
    info->closevnode = CloseVNode;
    info->read = Read;
    info->readlink = Readlink;
    info->write = write;
    info->getdents = GetDEnts;
    info->stat = Stat;
    info->name = Name;
    info->access = Access;
    info->chmod = Chmod;
    info->chown = Chown;
    info->link = Link;
    info->symlink = Symlink;
    info->rename = Rename;
    info->mknod = Mknod;
    info->mkdir = Mkdir;
    info->truncate = Truncate;
    info->fsync = FSync;
    info->fdatasync = FDataSync;
    info->unlink = Unlink;
    info->rmdir = Rmdir;
    info->creat = Creat;
    info->statfs = StatFS;
    info->size = Size;
    info->type = Type;
    info->utime = Utime;
    info->fcntl = Fcntl;
    info->ioctl = Ioctl;
    info->mmap = Mmap;


	fsmount->fs = info;

	ext2_privdata *privdata = new ext2_privdata;
	if (privdata == NULL){
		free(sblock);
		free(info);

		return -ENOMEM;
	}
	privdata->sblock = sblock;
	privdata->blkdev = blkdev;
	privdata->BlockSize = 1024 << sblock->s_log_block_size;
	privdata->DiskBlockLog = sblock->s_log_block_size + 1;
	privdata->IndirectBlockEntries = (privdata->BlockSize / 4);
	privdata->DiskBlocksPerFSBlock = privdata->BlockSize / 512;
	privdata->DirectBlocksAreaSize = privdata->BlockSize * EXT2_NDIR_BLOCKS;
	privdata->IndirectBlocksAreaSize = privdata->BlockSize * privdata->IndirectBlockEntries;
	privdata->DoubleIndirectBlocksAreaSize = privdata->IndirectBlocksAreaSize * privdata->IndirectBlockEntries;
	privdata->DirectIndirectBlocksAreaSize = privdata->DirectBlocksAreaSize + privdata->IndirectBlocksAreaSize;
	privdata->DirIndirDIndirBlocksAreaSize = privdata->DirectIndirectBlocksAreaSize +
						privdata->BlockSize * privdata->IndirectBlockEntries * privdata->IndirectBlockEntries;

	fsmount->privdata = (void *) privdata;


    ext2_inode *rootInode = readInode(2, privdata);
    if (IS_NULL_PTR(rootInode)) {
        free(sblock);
        free(info);

        return -ENOMEM;
    }

	VNode *root;
	VNodeManager::GetVnode(fsmount->mountId, 2, &root);
	root->mount = fsmount;
    root->privdata = rootInode;
	fsmount->fsRootVNode = root;

	return 0;
}

int Ext2::OpenFD(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int Ext2::CloseFD(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int Ext2::DupFD(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

//If I return a pointer I can't do easily distinctions between different errors
ext2_inode *Ext2::readInode(unsigned long id, ext2_privdata *privdata)
{
	int inodeIndex = ((uint32_t) (id - 1)) % privdata->sblock->s_inodes_per_group;
	int inodeGroup = ((uint32_t) (id - 1)) / privdata->sblock->s_inodes_per_group;

	char *groupTmpBlkBuff = (char *) malloc(512);
        if (groupTmpBlkBuff == 0){
	    return 0;
	}
	privdata->blkdev->ReadBlock(privdata->blkdev, 4, 1, (uint8_t *) groupTmpBlkBuff);
	ext2_group_desc *group = (ext2_group_desc *) groupTmpBlkBuff;

	#if 0
		printk("Inode table: %i\n", group->bg_inode_table);
	#endif

	//TODO: Warning: unchecked malloc
        //FIXME: after some calls it's going to crash here
	char *inodeTableTmpBlkBuf = (char *) malloc(privdata->sblock->s_inodes_per_group * privdata->sblock->s_inode_size);
	privdata->blkdev->ReadBlock(privdata->blkdev, group[inodeGroup].bg_inode_table * privdata->DiskBlocksPerFSBlock,
					(privdata->sblock->s_inodes_per_group * privdata->sblock->s_inode_size) / 512, (uint8_t *) inodeTableTmpBlkBuf);
	ext2_inode *first_table = (ext2_inode *) inodeTableTmpBlkBuf;

        free(group);
        ext2_inode *inode = new ext2_inode;
        memcpy(inode, &first_table[inodeIndex], sizeof(ext2_inode));
        free(first_table);

        return inode;
}

int Ext2::writeINode(VNode *node)
{
    ext2_inode *inode = getInode(node);
    ext2_privdata *privdata = (ext2_privdata *) node->mount->privdata;

    int inodeIndex = ((uint32_t) (node->vnid.id - 1)) % privdata->sblock->s_inodes_per_group;
    int inodeGroup = ((uint32_t) (node->vnid.id - 1)) / privdata->sblock->s_inodes_per_group;

    char *groupTmpBlkBuff = (char *) malloc(512);
    if (groupTmpBlkBuff == 0){
        return 0;
    }
    privdata->blkdev->ReadBlock(privdata->blkdev, 4, 1, (uint8_t *) groupTmpBlkBuff);
    ext2_group_desc *group = (ext2_group_desc *) groupTmpBlkBuff;

    char *inodeTableTmpBlkBuf = (char *) malloc(privdata->sblock->s_inodes_per_group * privdata->sblock->s_inode_size);
    privdata->blkdev->ReadBlock(privdata->blkdev, group[inodeGroup].bg_inode_table * privdata->DiskBlocksPerFSBlock,
                                (privdata->sblock->s_inodes_per_group * privdata->sblock->s_inode_size) / 512, (uint8_t *) inodeTableTmpBlkBuf);
    ext2_inode *first_table = (ext2_inode *) inodeTableTmpBlkBuf;

    memcpy(&first_table[inodeIndex], inode, sizeof(ext2_inode));

    privdata->blkdev->WriteBlock(privdata->blkdev, group[inodeGroup].bg_inode_table * privdata->DiskBlocksPerFSBlock,
                                (privdata->sblock->s_inodes_per_group * privdata->sblock->s_inode_size) / 512, (uint8_t *) inodeTableTmpBlkBuf);
    free(group);
    free(first_table);

    return 0;
}

//If I return a pointer I can't do easily distinctions between different errors
ext2_inode *Ext2::getInode(VNode *node)
{
    return (ext2_inode *) node->privdata;
}

int Ext2::ReadBlocksData(ext2_inode *inode, uint32_t *indirectBlocks, VNode *node, uint32_t pos, char *buffer, unsigned int bufsize)
{
	//Filesystem private data
	ext2_privdata *privdata = (ext2_privdata *) node->mount->privdata;

	//Buffer start pointer
	char *startBuf = buffer;

	//I start the file reading from the file current position
	unsigned int startBlock = pos / privdata->BlockSize;

	//Read an half block as start block
	if (((pos % privdata->BlockSize) || ((pos % privdata->BlockSize == 0) && (bufsize < privdata->BlockSize))) && (startBlock < privdata->IndirectBlockEntries)){
		#ifdef EXT2_READ_DEBUG
			printk("half block as start. pos: %i size: %i\n", pos, inode->i_size);
		#endif

		char *firstBlock = (char *) malloc(privdata->BlockSize);
		if (firstBlock == NULL) return -ENOMEM;

		privdata->blkdev->ReadBlock(privdata->blkdev, (indirectBlocks[startBlock] << privdata->DiskBlockLog), privdata->DiskBlocksPerFSBlock, (uint8_t *) firstBlock);

		int offset = pos % privdata->BlockSize;

		if (privdata->BlockSize - offset > bufsize){
			memcpy(buffer, firstBlock + offset, bufsize);
			buffer += bufsize;
		}else{
			memcpy(buffer, firstBlock + offset, privdata->BlockSize - offset);
			buffer += privdata->BlockSize - offset;
		}

		startBlock++;

		free(firstBlock);
	}

	if ((unsigned int) (buffer - startBuf) == bufsize){
		return bufsize;
	}else{
		bufsize -= buffer - startBuf;
	}

	//The end of direct read
	unsigned int endBlock = (bufsize / privdata->BlockSize) + startBlock;
	if (endBlock >= privdata->IndirectBlockEntries) endBlock = privdata->IndirectBlockEntries;

	#ifdef EXT2_READ_DEBUG
		printk("I read blocks! start: %i end: %i\n", startBlock, endBlock);
	#endif

	//I read direct blocks
	for (unsigned int i = startBlock; i < endBlock; i++){
		privdata->blkdev->ReadBlock(privdata->blkdev, (indirectBlocks[i] << privdata->DiskBlockLog), privdata->DiskBlocksPerFSBlock, (uint8_t *) buffer);
		buffer += privdata->BlockSize;

		if (indirectBlocks[i] == 0){
			printk("indirectBlocks[i] == 0: pos: %i, bufsize: %i, i: %i\n", pos, bufsize, i);
		}

		#ifdef EXT2_READ_DEBUG
			printk("Read block: %i[%i].\n", i, indirectBlocks[i]);
		#endif
	}

	//If the file isn't a multiple of a block and I haven't read all direct blocks
	if ((endBlock < privdata->IndirectBlockEntries) && ((bufsize % privdata->BlockSize) != 0)){
		#ifdef EXT2_READ_DEBUG
			printk("half block as end.\n");
		#endif

		char *lastBlock = (char *) malloc(privdata->BlockSize);
		if (lastBlock == NULL) return -ENOMEM;

		privdata->blkdev->ReadBlock(privdata->blkdev, (indirectBlocks[endBlock] << privdata->DiskBlockLog), privdata->DiskBlocksPerFSBlock, (uint8_t *) lastBlock);

		memcpy(buffer, lastBlock, bufsize % privdata->BlockSize);
		buffer += bufsize % privdata->BlockSize;

		free(lastBlock);
	}

	return buffer - startBuf;
}

int Ext2::ReadDoubleIndirectBlocksData(ext2_inode *inode, uint32_t *doubleIndirectBlock, VNode *node, uint32_t pos, char *buffer, unsigned int bufsize)
{
	//Filesystem private data
	ext2_privdata *privdata = (ext2_privdata *) node->mount->privdata;

	//Buffer start pointer
	char *startBuf = buffer;

	int initPos = pos;
	int i = pos / (privdata->IndirectBlockEntries * privdata->BlockSize);

	while(pos < initPos + bufsize){
		uint32_t *indirectBlocks = (uint32_t *) malloc(privdata->BlockSize);
		if (indirectBlocks == NULL) return -ENOMEM;

		privdata->blkdev->ReadBlock(privdata->blkdev, (doubleIndirectBlock[i] << privdata->DiskBlockLog), privdata->DiskBlocksPerFSBlock, (uint8_t *) indirectBlocks);

		buffer += ReadBlocksData(inode, indirectBlocks, node, pos % privdata->IndirectBlocksAreaSize, buffer, bufsize);
		pos += buffer - startBuf;
		bufsize -= buffer - startBuf;

		i++;

		free(indirectBlocks);
	}

	return buffer - startBuf;
}

int Ext2::ReadTripleIndirectBlocksData(ext2_inode *inode, uint32_t *tripleIndirectBlock, VNode *node, uint32_t pos, char *buffer, unsigned int bufsize)
{
	//Filesystem private data
	ext2_privdata *privdata = (ext2_privdata *) node->mount->privdata;

	//Buffer start pointer
	char *startBuf = buffer;

	int initPos = pos;
	int i = pos / (privdata->IndirectBlockEntries*privdata->IndirectBlockEntries*privdata->BlockSize);

	while(pos < initPos + bufsize){
		uint32_t *indirectBlocks = (uint32_t *) malloc(privdata->BlockSize);
		if (indirectBlocks == NULL) return -ENOMEM;

		privdata->blkdev->ReadBlock(privdata->blkdev, (tripleIndirectBlock[i] << privdata->DiskBlockLog), privdata->DiskBlocksPerFSBlock, (uint8_t *) indirectBlocks);

		buffer += ReadDoubleIndirectBlocksData(inode, indirectBlocks, node, pos % privdata->DoubleIndirectBlocksAreaSize, buffer, bufsize);
		pos += buffer - startBuf;
		bufsize -= buffer - startBuf;

		i++;

		free(indirectBlocks);
	}

	return buffer - startBuf;
}


int Ext2::ReadData(ext2_inode *inode, VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	//First I check If I'm above the end of file
	if (pos >= inode->i_size) return 0;

	//Filesystem private data
	ext2_privdata *privdata = (ext2_privdata *) node->mount->privdata;

	#ifdef EXT2_READ_DEBUG
		printk("Blocks: %i\n", inode->i_size / 1024);
	#endif

	//If the buffer is bigger than the file I read only
	//file's bytes
	if (bufsize + pos > inode->i_size) bufsize = inode->i_size - pos;

	int result = 0;

	//TODO: new code, should be tested
	if ((inode->i_size > 0) && (inode->i_blocks == 0)){
		memcpy(buffer, ((char *) inode->i_block) + pos, bufsize);

		return bufsize;
	}

	/*
	 Direct blocks
	*/
	if (pos < privdata->DirectBlocksAreaSize){
		if (bufsize + pos < privdata->DirectBlocksAreaSize){
			result = ReadBlocksData(inode, inode->i_block, node, pos, buffer, bufsize);
		}else{
			result = ReadBlocksData(inode, inode->i_block, node, pos, buffer, privdata->DirectBlocksAreaSize - pos);
		}

		if (pos + bufsize < privdata->DirectBlocksAreaSize) return result;
	}

	/*
	 Indirect blocks
        */
	if (pos < privdata->DirectIndirectBlocksAreaSize){
		uint32_t *indirectBlocks = (uint32_t *) malloc(privdata->BlockSize);
		if (indirectBlocks == NULL) return -ENOMEM;

		privdata->blkdev->ReadBlock(privdata->blkdev,
						inode->i_block[EXT2_IND_BLOCK] << privdata->DiskBlockLog,
						privdata->DiskBlocksPerFSBlock, (uint8_t *) indirectBlocks
					   );

		if (bufsize - result < privdata->DirectIndirectBlocksAreaSize){
			result += ReadBlocksData(inode, indirectBlocks, node, pos + result - privdata->DirectBlocksAreaSize,
							buffer + result, bufsize - result);
		}else{
			result += ReadBlocksData(inode, indirectBlocks, node, pos + result - privdata->DirectBlocksAreaSize,
							buffer + result, privdata->IndirectBlocksAreaSize);
		}

		free(indirectBlocks);

		if (pos + bufsize < privdata->DirectIndirectBlocksAreaSize) return result;
	}

	/*
         Double indirect blocks
        */
	if (pos < privdata->DirIndirDIndirBlocksAreaSize){
		uint32_t *doubleIndirectBlock = (uint32_t *) malloc(privdata->BlockSize);
		if (doubleIndirectBlock == NULL) return -ENOMEM;

		privdata->blkdev->ReadBlock(privdata->blkdev,
						inode->i_block[EXT2_DIND_BLOCK] << privdata->DiskBlockLog,
						privdata->DiskBlocksPerFSBlock,
						(uint8_t *) doubleIndirectBlock
					   );

		if (bufsize + pos < privdata->DirIndirDIndirBlocksAreaSize){
			result += ReadDoubleIndirectBlocksData(inode, doubleIndirectBlock, node,
								pos + result - privdata->DirectIndirectBlocksAreaSize, buffer + result,
								bufsize - result);
		}else{
			result += ReadDoubleIndirectBlocksData(inode, doubleIndirectBlock,
								node, pos + result - privdata->DirectIndirectBlocksAreaSize, buffer + result,
								privdata->DirIndirDIndirBlocksAreaSize - pos);
		}

		free(doubleIndirectBlock);

		if (pos + bufsize < privdata->DirIndirDIndirBlocksAreaSize) return result;
	}

	/*
	 Triple indirect blocks
	*/
	uint32_t *tripleIndirectBlock = (uint32_t *) malloc(privdata->BlockSize);
	if (tripleIndirectBlock == NULL) return -ENOMEM;

	privdata->blkdev->ReadBlock(privdata->blkdev,
					inode->i_block[EXT2_TIND_BLOCK] << privdata->DiskBlockLog,
					privdata->DiskBlocksPerFSBlock,
					(uint8_t *) tripleIndirectBlock
				   );

	result += ReadTripleIndirectBlocksData(inode, tripleIndirectBlock, node, pos + result - privdata->DirIndirDIndirBlocksAreaSize,
						buffer + result, bufsize - result);

	free(tripleIndirectBlock);


	#ifdef EXT2_READ_DEBUG
		printk("Result: %i\n", result);
	#endif

	return result;
}

inline bool isAligned(void *ptr, int align)
{
    return (((unsigned long) ptr) & (align - 1)) == 0;
}

int Ext2::findDirectoryEntry(VNode *dirNode, const char *name, unsigned long *inodeNumber,
                       unsigned long *recordOffset, unsigned long *prevRecordOffset, unsigned long *nextRecordOffset)
{
    DEBUG_MSG("Reading directory, looking for %s...\n", name);
    DEBUG_MSG("Block size is: %i\n", privdata->BlockSize);

    ext2_inode *inode = getInode(dirNode);
    ext2_privdata *privdata = (ext2_privdata *) dirNode->mount->privdata;

    if (!S_ISDIR(inode->i_mode)){
        return -ENOTDIR;
    }

    void *block = malloc(privdata->BlockSize);
    if (IS_NULL_PTR(block)) {
        return -ENOMEM;
    }
    ext2_dir_entry_2 *dirEntry = (ext2_dir_entry_2 *) block;

    unsigned long dirSize = inode->i_size;
    int blockIndex = -1;
    unsigned long prevRecordOff = 0;
    unsigned long currRecordOff = 0;
    unsigned long nextRecordOff = 0;
    unsigned long scannedBlockBytes;

    do {
        scannedBlockBytes = 0;
        blockIndex++;
        unsigned long availBytes = ReadData(inode, dirNode, blockIndex * privdata->DiskBlocksPerFSBlock, (char *) block, privdata->BlockSize);
        do {
            if (UNLIKELY(!isAligned(dirEntry, 4) || (dirEntry->rec_len < dirEntry->name_len + 8) || //8 = other fileds
                         (((unsigned long) dirEntry) - ((unsigned long) block) + dirEntry->rec_len > privdata->BlockSize) ||
                         (dirEntry->name_len == 0) || (dirEntry->file_type > EXT2_FT_SYMLINK))) {
                printk("Ext2::findDirectoryEntry: corrupted record entry.\n");
                return -EIO;
            }

            nextRecordOff = currRecordOff + dirEntry->rec_len;

            //If the first part of the name is the same but is longer than dir->name_len
            //we want to reconize the name as different
	    //dir->name *isn't a null terminated* string
            if (!strncmp(name, dirEntry->name, dirEntry->name_len) && (name[dirEntry->name_len] == 0)){
                *inodeNumber = dirEntry->inode;
                if (recordOffset != NULL) {
                    *recordOffset = currRecordOff;
                }
                if (prevRecordOffset != NULL) {
                    *prevRecordOffset = prevRecordOff;
                }
                if (nextRecordOffset != NULL) {
                    *nextRecordOffset = nextRecordOff;
                }
                free(block);
                DEBUG_MSG("Found entry: %s. Returning.\n", name);
                return 0;
            }

            scannedBlockBytes += dirEntry->rec_len;
            dirEntry = (ext2_dir_entry_2 *) ((unsigned long) dirEntry + dirEntry->rec_len);
            prevRecordOff = currRecordOff;
            currRecordOff = nextRecordOff;

        } while (scannedBlockBytes < availBytes);
    } while (blockIndex * privdata->BlockSize + scannedBlockBytes < dirSize);

    free(block);
    DEBUG_MSG("Entry %s not found.\n");
    return -ENOENT;
}

int Ext2::iNodeNumberToVNode(unsigned long id, FSMount *mount, VNode **vnd)
{
    VNodeManager::GetVnode(mount->mountId, id, vnd);
    if ((*vnd)->privdata == NULL) {
        ext2_inode *fileInode = readInode(id, (ext2_privdata *) mount->privdata);
        if (IS_NULL_PTR(fileInode)) {
            return -ENOMEM;
        }

        (*vnd)->mount = mount;
        (*vnd)->privdata = fileInode;
    }

    return 0;
}

int Ext2::Lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
    DEBUG_MSG("Ext2::lookup: %s\n", name);

    unsigned long inodeNumber = 0;
    int res = findDirectoryEntry(node, name, &inodeNumber, NULL, NULL, NULL);
    if (UNLIKELY(res < 0)) {
        return res;
    }

    res = iNodeNumberToVNode(inodeNumber, node->mount, vnd);
    if (LIKELY(res >= 0)) {
        *ntype = getInode(node)->i_mode;
    }
    return res;
}

int Ext2::Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	ext2_inode *inode = getInode(node);

	if (S_ISDIR(inode->i_mode)) return -EISDIR;

	return ReadData(inode, node, pos, buffer, bufsize);
}

int Ext2::Readlink(VNode *node, char *buffer, size_t bufsize)
{
	ext2_inode *inode = getInode(node);

	if (!S_ISLNK(inode->i_mode)) return -EINVAL;

	return ReadData(inode, node, 0, buffer, bufsize);
}

int Ext2::GetDEnts(VNode *node, dirent *dirp, unsigned int count)
{
	ext2_inode *inode = getInode(node);

	if (!S_ISDIR(inode->i_mode)) return -ENOTDIR;

	ext2_dir_entry_2 *dir = (ext2_dir_entry_2 *) malloc(inode->i_size);
	if (dir == NULL) return -ENOENT;
	ReadData(inode, node, 0, (char *) dir, inode->i_size);

	unsigned int bufferUsedBytes = 0;
	unsigned int readBytes = 0;

	do{
	    if (dir->name_len != 0){
		rawstrcpy(dirp->d_name, dir->name, sizeof(dirp->d_name), dir->name_len + 1);
		dirp->d_reclen = sizeof(dirent);
		dirp->d_off = sizeof(dirent); //TODO: ci andrebbe pos
		bufferUsedBytes += dirp->d_reclen;
 
		dirp = (struct dirent *) (((unsigned long) dirp) + dirp->d_reclen); 
            }
		readBytes += dir->rec_len;
		dir = (ext2_dir_entry_2 *) ((unsigned long) dir + dir->rec_len);
	//TODO: < o <=?
	}while((readBytes < inode->i_size) && (bufferUsedBytes + sizeof(dirent) < count));

	//return 0 on end of directory, else read bytes
	return /*(dir->inode) ? */bufferUsedBytes/* : 0*/;
}

int Ext2::Stat(VNode *node, struct stat *buf)
{
	//Filesystem private data
	ext2_privdata *privdata = (ext2_privdata *) node->mount->privdata;

	ext2_inode *inode = getInode(node);

	buf->st_dev = 0;
	buf->st_ino = node->vnid.id;
	buf->st_mode = inode->i_mode;
	buf->st_nlink = inode->i_links_count;
	buf->st_uid = inode->i_uid;
	buf->st_gid = inode->i_gid;
	buf->st_rdev = 0;
	buf->st_size = inode->i_size;
	buf->st_blksize = privdata->BlockSize;
	buf->st_blocks = inode->i_blocks;
	buf->st_atime = inode->i_atime;
	buf->st_mtime = inode->i_mtime;
	buf->st_ctime = inode->i_ctime;

	return 0;
}

int Ext2::Access(VNode *node, int aMode, int uid, int gid)
{
	ext2_inode *inode = getInode(node);

	//Write only
	//No execute

	mode_t tmpIMode = inode->i_mode;

	if (uid == 0){
		tmpIMode |= (06 | ((tmpIMode >> 14) & 1));

	}else if (uid == inode->i_uid){
	 	tmpIMode >>= 6;

	}else if (gid == inode->i_gid){
		tmpIMode >>= 3;
	}

	if (aMode & ~(tmpIMode & 07)) return -EACCES;
	
	return 0;
}

int Ext2::Chmod(VNode *node, mode_t mode)
{
    ext2_inode *inode = getInode(node);

    node->dirty = true;
    inode->i_mode = mode;

    return 0;
}

int Ext2::Chown(VNode *node, uid_t uid, gid_t gid)
{
    ext2_inode *inode = getInode(node);

    node->dirty = true;
    inode->i_uid = uid;
    inode->i_gid = gid;

    return 0;
}

int Ext2::Name(VNode *directory, VNode *node, char **name, int *len)
{
	#ifdef EXT2_DEBUG
		printk("Lookup %s\n", name);
	#endif

	ext2_inode *inode = getInode(directory);

	if (!S_ISDIR(inode->i_mode)){
		printk("I can only do lookups on a directory\n");

		return -ENOTDIR;
	}

	ext2_dir_entry_2 *dir = (ext2_dir_entry_2 *) malloc(inode->i_size);
	if (dir == NULL) return -ENOMEM;
	ReadData(inode, node, 0, (char *) dir, inode->i_size); 

	unsigned int readBytes = 0;

	do{
		#ifdef EXT2_DEBUG
			printk("%s\n", dir->name);
		#endif

		//If the first part of the name is the same but is longer than dir->name_len
		//we want to reconize the name as different
		//dir->name *isn't a null terminated* string
		if (dir->inode == node->vnid.id){
			*name = strndup(dir->name, dir->name_len);
            *len = dir->name_len;
			return 0;
		}

		readBytes += dir->rec_len;
		dir = (ext2_dir_entry_2 *) ((unsigned long) dir + dir->rec_len);
	}while((readBytes < inode->i_size) && (dir->inode));

	///printk("ENOENT: (node addr: %x)\n", node);

	return -ENOENT;
}

int Ext2::Umount(VNode *root)
{
    return 0;
}

int Ext2::CloseVNode(VNode *node)
{
    int ret = 0;
    if (node->dirty) {
        ret = writeINode(node);

        if (getInode(node)->i_links_count == 0) {
             printk("need to cleanup inode: %i\n", node->vnid.id);
        }
    }
    free(node->privdata);

    return ret;
}

int Ext2::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize, WriteOpFlags write)
{   
    return -EBADF;
}

int Ext2::Link(VNode *directory, VNode *oldNode, const char *newName)
{
    return -EROFS;
}

int Ext2::Symlink(VNode *directory, const char *oldName, const char *newName)
{
    return -EROFS;
}

int Ext2::Rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName)
{
    ext2_privdata *privdata = (ext2_privdata *) oldDirectory->mount->privdata;

    unsigned long inodeNumber = 0;
    unsigned long recordOff = 0;
    unsigned long prevRecordOff = 0;
    unsigned long nextRecordOff = 0;
    int res = findDirectoryEntry(oldDirectory, oldName, &inodeNumber, &recordOff, &prevRecordOff, &nextRecordOff);
    if (UNLIKELY(res < 0)) {
        return res;
    }

    void *block = malloc(privdata->BlockSize);
    unsigned long availBytes = ReadData(getInode(oldDirectory), oldDirectory, 0/*blockIndex * privdata->DiskBlocksPerFSBlock*/, (char *) block, privdata->BlockSize);

    ext2_dir_entry_2 *dirEntry = (ext2_dir_entry_2 *) ((unsigned long) block + recordOff);

    int newNameLen = strlen(newName);
    if ((newNameLen + 8 <= dirEntry->rec_len) && (oldDirectory == newDirectory)) {
        memcpy(dirEntry->name, newName, newNameLen);
        dirEntry->name_len = newNameLen;

    } else {
        return -EROFS;
        ext2_dir_entry_2 *prevDirEntry = (ext2_dir_entry_2 *) ((unsigned long) block + prevRecordOff);
        prevDirEntry->rec_len += (nextRecordOff - recordOff);
    }

    // small hack: getInode(directory)->i_block[0]
    privdata->blkdev->WriteBlock(privdata->blkdev, getInode(oldDirectory)->i_block[0] << privdata->DiskBlockLog, privdata->DiskBlocksPerFSBlock, (uint8_t *) block);

    return 0;
}

int Ext2::Mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev)
{
    return -EROFS;
}

int Ext2::Mkdir(VNode *directory, const char *newName, mode_t mode)
{
    return -EROFS;
}

int Ext2::Truncate(VNode *node, uint64_t length)
{
    return -EROFS;
}

int Ext2::FSync(VNode *node)
{
    return 0;
}

int Ext2::FDataSync(VNode *node)
{
    return 0;
}

int Ext2::Unlink(VNode *directory, const char *name)
{
    ext2_privdata *privdata = (ext2_privdata *) directory->mount->privdata;

    unsigned long inodeNumber = 0;
    unsigned long recordOff = 0;
    unsigned long prevRecordOff = 0;
    unsigned long nextRecordOff = 0;
    int res = findDirectoryEntry(directory, name, &inodeNumber, &recordOff, &prevRecordOff, &nextRecordOff);
    if (UNLIKELY(res < 0)) {
        return res;
    }

    void *block = malloc(privdata->BlockSize);
    unsigned long availBytes = ReadData(getInode(directory), directory, 0/*blockIndex * privdata->DiskBlocksPerFSBlock*/, (char *) block, privdata->BlockSize); 

    ext2_dir_entry_2 *dirEntry = (ext2_dir_entry_2 *) ((unsigned long) block + prevRecordOff);
    if (dirEntry->file_type == EXT2_FT_DIR) {
        return -EISDIR;
    }

    dirEntry->rec_len += (nextRecordOff - recordOff);
    // small hack: getInode(directory)->i_block[0]
    privdata->blkdev->WriteBlock(privdata->blkdev, getInode(directory)->i_block[0] << privdata->DiskBlockLog, privdata->DiskBlocksPerFSBlock, (uint8_t *) block);
    free(block);

    VNode *vnd;
    res = iNodeNumberToVNode(inodeNumber, directory->mount, &vnd);
    if (LIKELY(res < 0)) {
        return res;
    }
    vnd->dirty = true;
    ext2_inode *inode = getInode(vnd);
    unsigned int ntype = inode->i_mode;
    inode->i_links_count--;

    VNodeManager::PutVnode(vnd);

    return 0;
}

int Ext2::Rmdir(VNode *directory, const char *name)
{
    return -EROFS;
}


int Ext2::Creat(VNode *directory, const char *name, mode_t mode)
{
    return 0;
}

int Ext2::StatFS(VNode *directory, struct statfs *buf)
{
    return 0;
}

int Ext2::Size(VNode *node, int64_t *size)
{
    ext2_inode *inode = getInode(node);
    *size = inode->i_size;

    return 0;
}

int Ext2::Type(VNode *node, int *type)
{
    ext2_inode *inode = getInode(node);
    *type = inode->i_mode & S_IFMT;

    return 0;
}

int Ext2::Utime(VNode *node, const struct utimbuf *buf)
{
    ext2_inode *inode = getInode(node);

    node->dirty = true;
    inode->i_atime = buf->actime;
    inode->i_mtime = buf->modtime;

    return 0;
}

int Ext2::Fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int Ext2::Ioctl(VNode *node, int request, long arg)
{
    return -EIOCTLNOTSUPPORTED;
}

void *Ext2::Mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return 0;
}
