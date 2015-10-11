/***************************************************************************
 *   Copyright 2005 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: ramdisk.cpp                                                     *
 *   Date: 25/04/2005                                                      *
 ***************************************************************************/

#include <arch.h>

#ifndef ARCH_MIPS
#include <boot/bootloaderinfo.h>
#else
#include <arch/mips/boot/bootinfo.h>
#endif
#include <drivers/blockdevicemanager.h>
#include <cstdlib.h>
#include <errors.h>

#include <core/printk.h>
#include <drivers/ramdisk.h>

uint8_t *diskMem;

void RamDisk::init()
{
#ifndef BOOTINFO
       if (BootLoaderInfo::modulesCount() > 0){
           diskMem = (uint8_t *) BootLoaderInfo::module(0);
#else
           diskMem = (uint8_t *) BootInfo::ramdisk();
#endif
           BlockDevice *ramd0 = new BlockDevice;
           //TODO: Warning: unchecked malloc
           ramd0->int_cookie = 0;
           ramd0->name = "ramd0";
           ramd0->Major = 0;
           ramd0->Minor = 0;
           ramd0->ReadBlock = readBlock;
           ramd0->WriteBlock = writeBlock;
           ramd0->read = read;
           ramd0->write = write;
           ramd0->ioctl = ioctl;
           ramd0->mmap = mmap;

           BlockDeviceManager::Register(ramd0);
#ifndef BOOTINFO
       }
#endif
}

void RamDisk::readBlock(BlockDevice *bd, int block, int blockn, uint8_t *blockbuffer)
{
    memcpy(blockbuffer, diskMem + block*512, blockn*512);
}

bool RamDisk::writeBlock(BlockDevice *bd, int block, int blocksN, uint8_t *blockbuffer)
{
    memcpy(diskMem + block*512, blockbuffer, blocksN*512);
    return true;
}

int RamDisk::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
#ifndef BOOTINFO
    if (pos >= BootLoaderInfo::moduleSize(0)){
        return 0;
    }

    if (pos + bufsize >= BootLoaderInfo::moduleSize(0)){
        bufsize = BootLoaderInfo::moduleSize(0) - pos;
    }
#endif

    memcpy(buffer, diskMem + pos, bufsize);
    return bufsize;
}

int RamDisk::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
#ifndef BOOTINFO
    if (pos >= BootLoaderInfo::moduleSize(0)){
        return 0;
    }

    if (pos + bufsize >= BootLoaderInfo::moduleSize(0)){
        bufsize = BootLoaderInfo::moduleSize(0) - pos;
    }
#endif

    memcpy(diskMem + pos, buffer, bufsize);
    return bufsize;
}

int RamDisk::ioctl(VNode *node, int request, long arg)
{
    return -EIOCTLNOTSUPPORTED;
}

void *RamDisk::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return (void *) -EINVAL;
}

