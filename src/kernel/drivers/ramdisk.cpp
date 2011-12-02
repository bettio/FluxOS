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

#include <boot/bootloaderinfo.h>
#include <drivers/blockdevicemanager.h>
#include <cstdlib.h>

#include <core/printk.h>
#include <drivers/ramdisk.h>

uint8_t *diskMem;

void RamDisk::init()
{
       if (BootLoaderInfo::modulesCount() > 0){
           diskMem = (uint8_t *) BootLoaderInfo::module(0);
           BlockDevice *ramd0 = new BlockDevice;
           //TODO: Warning: unchecked malloc
           ramd0->int_cookie = 0;
           ramd0->name = "ramd0";
           ramd0->Major = 0;
           ramd0->Minor = 0;
           ramd0->ReadBlock = readBlock;
           BlockDeviceManager::Register(ramd0);
       }
}

void RamDisk::readBlock(BlockDevice *bd, int block, int blockn, uint8_t *blockbuffer)
{
    memcpy(blockbuffer, diskMem + block*512, blockn*512);
}

