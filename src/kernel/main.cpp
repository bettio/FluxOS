/***************************************************************************
 *   Copyright 2004,2011 by Davide Bettio <davide.bettio@kdemail.net>      *
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
 *   Name: main.cpp                                                        *
 *   Date: 6/09/2004                                                       *
 ***************************************************************************/

#include <arch.h>
#include <main.h>
#include <core/printk.h>
#include <cstdlib.h>

#include <core/archmanager.h>
#include <core/systemerrors.h>

#include <drivers/chardevicemanager.h>
#include <drivers/blockdevicemanager.h>
#include <drivers/diskpart.h>
#include <drivers/ramdisk.h>
#include <drivers/fulldev.h>
#include <drivers/nulldev.h>
#include <drivers/zerodev.h>

#include <filesystem/procfs/procfs.h>
#include <filesystem/ext2/ext2.h>
#include <filesystem/tmpfs/tmpfs.h>
#include <filesystem/devfs/devfs.h>

#include <filesystem/vfs.h>
#include <filesystem/vnodemanager.h>

#include <net/net.h>

#include <uapi/memoryuapi.h>
#include <uapi/processuapi.h>

int main()
{
	ArchManager::Init();
    
	printk("I'm vmflux kernel.\n");
	printk("FluxOS 0.1\n");
   
    Net::init();

	ArchManager::InitArch();

	ArchManager::InitMemoryManagment();
	
	CharDeviceManager::Init();
	BlockDeviceManager::Init();
	FileSystem::DevFS::Init();
        FullDev::init();
        NullDev::init();
        ZeroDev::init();
	DiskPart::Init();
        RamDisk::init();

	FileSystem::VFS::Init();

	ArchManager::InitHardware();

	FileSystem::ProcFS::Init();
	FileSystem::Ext2::Init();
	FileSystem::TmpFS::Init();
	FileSystem::DevFS::RegisterAsFileSystem();

        const char *rootDevice = "/dev/ramd0";
        const char *rootFSType = "ext2";
        if (FileSystem::VFS::Mount(rootDevice, "/", rootFSType, 0, 0) < 0) {
            printk("root=%s, fstype=%s\n", rootDevice, rootFSType);
            kernelPanic("cannot mount root filesystem.");
        }
        FileSystem::VFS::Mount("null", "/dev/", "devfs", 0, 0);

	ArchManager::InitMultitasking();

        ProcessUAPI::init();
        MemoryUAPI::init();

	printk("Starting Init...\n");

        ArchManager::StartInit();
	while(1);
}
