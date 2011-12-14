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
 *   Name: chardevicemanager.cpp                                           *
 *   Date: 27/12/2006                                                      *
 ***************************************************************************/

#include <drivers/chardevicemanager.h>
#include <drivers/devid.h>
#include <filesystem/devfs/devfs.h>
#include <core/printk.h>
#include <cstring.h>
#include <cstdlib.h>

#include <QHash>
#include <QString>

QHash<DevId, CharDevice *> CharDevices;
QHash<QString, CharDevice *> CharDevicesByName;

void CharDeviceManager::Init()
{
    CharDevices = QHash<DevId, CharDevice *>();
    CharDevicesByName = QHash<QString, CharDevice *>();
}

int CharDeviceManager::Register(CharDevice *cd)
{
    if (cd == NULL){
        printk("cd is NULL. Char device registration failed.\n");
        return 0;
    }

    DevId chrId(cd->Major, cd->Minor);
    CharDevice *existingDev = CharDevices.value(chrId);
    if (existingDev != 0){
       printk("Error: Major and Minor already used by /dev/%s\n", existingDev->name);
       return 0;
    }
        
    CharDevices.insert(chrId, cd);
    CharDevicesByName.insert(cd->name, cd);

    FileSystem::DevFS::Mknod(0, cd->name, S_IFCHR, (cd->Major << 16) | cd->Minor);

    printk("Registered block device %s.\n", cd->name);

    return 0;
}

int CharDeviceManager::Unregister(const char *name)
{
    CharDevice *dev = CharDevicesByName.take(name);
    if (dev == NULL){
        printk("Requested unregister for unknown char device %s", name);
        return 0;
    }
    CharDevices.remove(DevId(dev->Major, dev->Minor));

    return 0;
}

CharDevice *CharDeviceManager::Device(char *name)
{
    return CharDevicesByName.value(name);
}

CharDevice *CharDeviceManager::Device(int major, int minor)
{
    DevId blkId(major, minor);
    
    return CharDevices.value(blkId);
}
