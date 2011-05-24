/*/***************************************************************************
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
 *   Name: blockdevicemanager.cpp                                           *
 *   Date: 30/08/2006                                                      *
 ***************************************************************************/


#include <drivers/blockdevicemanager.h>
#include <drivers/devid.h>
#include <filesystem/devfs/devfs.h>
#include <string.h>
#ifdef USE_GLIBC
#include <stdlib.h>
#endif
#include <core/printk.h>

#include <QHash>
#include <QString>

QHash<DevId, BlockDevice *> BlockDevices;
QHash<QString, BlockDevice *> BlockDevicesByName;

void BlockDeviceManager::Init()
{
    BlockDevices = QHash<DevId, BlockDevice *>();
    BlockDevicesByName = QHash<QString, BlockDevice *>();
}

int BlockDeviceManager::Register(BlockDevice *bd)
{
    if (bd == 0){
        printk("bd is NULL. Block device registration failed.\n");
        return 0;
    }

    BlockDevices.insert(DevId(bd->Major, bd->Minor), bd);
    BlockDevicesByName.insert(bd->name, bd);

    FileSystem::DevFS::Mknod(0, bd->name, S_IFBLK, (bd->Major << 16) | bd->Minor);

    printk("Registered block device %s.\n", bd->name);

    return 0;
}

int BlockDeviceManager::Unregister(const char *name)
{
    BlockDevice *dev = BlockDevicesByName.take(name);
    if (dev == NULL){
        printk("Requested unregister for unknown block device %s", name);
        return 0;
    }
    BlockDevices.remove(DevId(dev->Major, dev->Minor));

    return 0;
}

BlockDevice *BlockDeviceManager::Device(char *name)
{
    return BlockDevicesByName.value(name);
}

BlockDevice *BlockDeviceManager::Device(int major, int minor)
{
    DevId blkId(major, minor);
    
    return BlockDevices.value(blkId);
}
