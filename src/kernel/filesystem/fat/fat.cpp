/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: fat.cpp                                                         *
 *   Date: 12/09/2010                                                      *
 ***************************************************************************/

#include <cstdlib.h>
#include <cstring.h>

#include <filesystem/vfs.h>
#include <filesystem/fat/fat.h>
#include <filesystem/vnodemanager.h>
#include <drivers/blockdevicemanager.h>
#include <core/printk.h>

#define END_OF_FILE 0xFFFFFF8
#define IS_END_OF_FILE(c) (c >= END_OF_FILE)
#define FAT32_CLUSTER_MASK 0xFFFFFFF
#define FAT32_CLUSTER(c) (c & FAT32_CLUSTER_MASK)

using namespace FileSystem;

int Fat::Init()
{
    FileSystemInfo *info = new FileSystemInfo;
    if (info == NULL) return -ENOMEM;
    info->name = "fat";

    info->mount = Mount;

    FileSystem::VFS::RegisterFileSystem(info);

    return 0;
}

uint16_t READ_INT8(char *buf, int offset)
{
    return ((uint8_t) (buf[offset]));
}

uint16_t READ_INT16(char *buf, int offset)
{
    return *((uint16_t *) (buf + offset));
}

uint32_t READ_INT32(char *buf, int offset)
{
    return *((uint32_t *) (buf + offset));
}


int Fat::Mount(FSMount *fsmount, BlockDevice *blkdev)
{  
    //------
    void *tmpblk = (void *) malloc(512);
    if (tmpblk == NULL) return -ENOMEM;

    blkdev->ReadBlock(blkdev, 0, 1, (uint8_t *) tmpblk);

    //------

    if (READ_INT16((char *) tmpblk, 0x0B) != 512){
        printk("Not supported sector size\n"); 
        return -EINVAL;
    }

    FSModuleInfo *info = new FSModuleInfo;
    if (info == NULL){
        return -ENOMEM;
    }
    info->umount = Umount;
    info->lookup = Lookup;
    info->closevnode = CloseVNode;
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
    info->utime = Utime;
    info->fcntl = Fcntl;
    info->ioctl = Ioctl;
    info->mmap = Mmap;


    fsmount->fs = info;

    FatPrivData *privdata = new FatPrivData;
    if (privdata == NULL){
        free(info);

        return -ENOMEM;
    }
    privdata->blkdev = blkdev;
    privdata->rootDirectoryCluster = READ_INT32((char *) tmpblk, 0x2C);
    privdata->fatsNumber = READ_INT16((char *) tmpblk, 0x10);
    privdata->sectorsPerFat = READ_INT32((char *) tmpblk, 0x24);
    privdata->sectorsPerCluster = READ_INT8((char *) tmpblk, 0x0D);
    privdata->bytesPerCluster = privdata->sectorsPerCluster * 512;
    privdata->reservedSectors = READ_INT16((char *) tmpblk, 0x0E);
    
    fsmount->privdata = (void *) privdata;

    VNode *root;
    VNodeManager::GetVnode(fsmount->mountId, 1, &root);

    root->mount = fsmount;
    root->vnid.id = 2;
    root->vnid.mountId = fsmount->mountId;

    fsmount->fsRootVNode = root;

    return 0;
}

uint64_t clusterToSector(FatPrivData *fs, uint32_t cluster)
{
    return (cluster - 2) * fs->sectorsPerCluster + fs->reservedSectors + fs->fatsNumber * fs->sectorsPerFat;
}

int Fat::ReadData(void *privdata, uint32_t firstCluster, uint32_t off, uint32_t bufsize, char *buf)
{
    FatPrivData *fs = (FatPrivData *) privdata;
    char *tmpBuf = (char *) malloc(fs->bytesPerCluster);
    int availBuf = bufsize;
    int currentOffset = off;
    
    if (off < fs->bytesPerCluster){
        uint64_t sector = clusterToSector(fs, firstCluster);
        fs->blkdev->ReadBlock(fs->blkdev, sector, fs->sectorsPerCluster, (uint8_t *) tmpBuf);
        int r = (bufsize + currentOffset > fs->bytesPerCluster) ? fs->bytesPerCluster - currentOffset : bufsize;
        memcpy(buf, tmpBuf + currentOffset, r);
        
        availBuf -= r;
        currentOffset += r;
    }


    int currentCluster = firstCluster;
    int clusterCounter = 1;
    while(availBuf){
        uint32_t *fatBuf = (uint32_t *) malloc(fs->bytesPerCluster);
        fs->blkdev->ReadBlock(fs->blkdev, fs->reservedSectors, fs->sectorsPerCluster, (uint8_t *) fatBuf);

        currentCluster = FAT32_CLUSTER(fatBuf[currentCluster]);
        if (IS_END_OF_FILE(currentCluster)){
          return bufsize - availBuf;
        }
        
        if (clusterCounter == (currentOffset / fs->bytesPerCluster)){
            fs->blkdev->ReadBlock(fs->blkdev, clusterToSector(fs, currentCluster), fs->sectorsPerCluster, (uint8_t *) tmpBuf);
        
            int r = (bufsize + (currentOffset % fs->bytesPerCluster) > fs->bytesPerCluster) ? fs->bytesPerCluster - (currentOffset % fs->bytesPerCluster) : bufsize;
            memcpy(buf + (currentOffset - off), tmpBuf + (currentOffset % fs->bytesPerCluster), r);
            
            availBuf -= r;
            currentOffset += r;
        }
        clusterCounter++;
    }

    return bufsize - availBuf;
}

int utf8UnicodeNCmp(const char *utf8Str, const uint16_t *unicodeStr, int n)
{
    for (int i = 0; i < n; i++){
      if (utf8Str[i] == unicodeStr[i]){
        if (utf8Str[i] == '\0'){
          return 0;
        }
      }else{
        return 1;
      }
    }
    
    return 0;
}

int Fat::Lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
    char *tmpBuf = (char *) malloc(2048);
    ReadData(node->mount->privdata, node->vnid.id, 0, 2048, tmpBuf);

    int i = 0;
    while(tmpBuf[i*32]){
        char nam[12];
        strncpy(nam, tmpBuf + i*32, 12);

        bool prevIsLongName = false;
        if (READ_INT8(tmpBuf, i*32 ) != 0xE5){

        if (READ_INT8(tmpBuf, i*32 + 0x0B) == 0x0F){
          int nameLen = strlen(name);
          int seqNum = READ_INT8(tmpBuf, i*32) & 0x0F;
          bool isLast = ((READ_INT8(tmpBuf, i*32) & 0xF0) == 0x40);
          if (isLast && (nameLen >= ((seqNum - 1)* 13)) && (nameLen <= (seqNum * 13))){
              prevIsLongName = true;

              //WARNING: ALLIGN

              if (!utf8UnicodeNCmp(name + ((seqNum - 1)* 13), (uint16_t *) (tmpBuf + i*32 + 1), 5)){
                uint32_t cluster = ((uint32_t) READ_INT16(tmpBuf + (i + seqNum)*32, 0x14)) << 16 | ((uint32_t) READ_INT16(tmpBuf + (i + seqNum)*32, 0x1A));
                VNodeManager::GetVnode(node->mount->mountId, cluster, vnd);

                
                (*vnd)->mount = node->mount;
                *ntype = 0777;
                
                return 0;                
              }
          }else if (isLast){
              prevIsLongName = true;
              i += seqNum; //oppure - 1
          }
        }else if (!prevIsLongName){
            if (!strncmp(name, nam, 12)){
                uint32_t cluster = ((uint32_t) READ_INT16(tmpBuf + i*32, 0x14)) << 16 | ((uint32_t) READ_INT16(tmpBuf + i*32, 0x1A));
                VNodeManager::GetVnode(node->mount->mountId, cluster, vnd);

                
                (*vnd)->mount = node->mount;
                *ntype = 0777;
                
                return 0;
            }
            prevIsLongName = false;
        }
        }

        i++;
    }

    return -ENOENT;
}

int Fat::Read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    return ReadData(node->mount->privdata, node->vnid.id, pos, bufsize, buffer);
}

int Fat::Readlink(VNode *node, char *buffer, size_t bufsize)
{
    return -EINVAL;
}

//TODO
int Fat::GetDEnts(VNode *node, dirent *dirp, unsigned int count)
{
}

//TODO
int Fat::Stat(VNode *node, struct stat *buf)
{
}

//TODO
int Fat::Access(VNode *node, int aMode, int uid, int gid)
{
}

int Fat::Chmod(VNode *node, mode_t mode)
{
    return -EPERM;
}

int Fat::Chown(VNode *node, uid_t uid, gid_t gid)
{
    return -EPERM;
}

//TODO
int Fat::Name(VNode *directory, VNode *node, char **name, int *len)
{
}

int Fat::Umount(VNode *root)
{
    return 0;
}

int Fat::CloseVNode(VNode *node)
{
    return 0;
}

//TODO
int Fat::Write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{   
    return -EBADF;
}

int Fat::Link(VNode *directory, VNode *oldNode, const char *newName)
{
    return -EPERM;
}

int Fat::Symlink(VNode *directory, const char *oldName, const char *newName)
{
    return -EPERM;
}

//TODO
int Fat::Rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName)
{
    return -EROFS;
}

int Fat::Mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev)
{
    return -EPERM;
}

//TODO
int Fat::Mkdir(VNode *directory, const char *newName, mode_t mode)
{
    return -EROFS;
}

//TODO
int Fat::Truncate(VNode *node, uint64_t length)
{
    return -EROFS;
}

//TODO
int Fat::FSync(VNode *node)
{
    return 0;
}

//TODO
int Fat::FDataSync(VNode *node)
{
    return 0;
}

//TODO
int Fat::Unlink(VNode *directory, const char *name)
{
    return -EROFS;
}

//TODO
int Fat::Rmdir(VNode *directory, const char *name)
{
    return -EROFS;
}

//TODO
int Fat::Creat(VNode *directory, const char *name, mode_t mode)
{
    return 0;
}

//TODO
int Fat::StatFS(VNode *directory, struct statfs *buf)
{
    return 0;
}

//TODO
int Fat::Utime(VNode *node, const struct utimbuf *buf)
{

    return 0;
}

int Fat::Fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int Fat::Ioctl(VNode *node, int request, long arg)
{
    return -EINVAL;
}

void *Fat::Mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return 0;
}
