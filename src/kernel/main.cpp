/***************************************************************************
 *   Copyright 2004 by Davide Bettio <davide.bettio@kdemail.net>           *
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

#include <drivers/chardevicemanager.h>
#include <drivers/blockdevicemanager.h>

#include <filesystem/procfs/procfs.h>
#include <filesystem/ext2/ext2.h>
#include <filesystem/tmpfs/tmpfs.h>
#include <filesystem/devfs/devfs.h>

#include <filesystem/vfs.h>
#include <filesystem/vnodemanager.h>


#include <core/system.h>

#include <drivers/diskpart.h>

#define BOOT 0

int main()
{
	ArchManager::Init();
    
	printk("I'm vmflux kernel.\n");
	printk("FluxOS 0.1\n");
    
	ArchManager::InitArch();

	ArchManager::InitMemoryManagment();
	
	CharDeviceManager::Init();
	BlockDeviceManager::Init();
	FileSystem::DevFS::Init();
	DiskPart::Init();

	FileSystem::VFS::Init();

	ArchManager::InitHardware();

	FileSystem::ProcFS::Init();
	FileSystem::Ext2::Init();
	FileSystem::TmpFS::Init();
	FileSystem::DevFS::RegisterAsFileSystem();

    #if BOOT == 0
    FileSystem::VFS::Mount("null", "/", "tmpfs", 0, 0);
    VNode *node;
    FileSystem::VFS::RelativePathToVnode(0, "/", &node);
    FS_CALL(node, mkdir)(node, "dev", 0);
    FS_CALL(node, mkdir)(node, "proc", 0);
    FS_CALL(node, mkdir)(node, "tmp", 0);
    FileSystem::VNodeManager::PutVnode(node);
    #else
	FileSystem::VFS::Mount("/dev/fd0", "/", "ext2", 0, 0);
	#endif

	FileSystem::VFS::Mount("null", "/dev/", "devfs", 0, 0);
	FileSystem::VFS::Mount("null", "/tmp/", "tmpfs", 0, 0);

    FileSystem::VFS::Mount("null", "/proc", "procfs", 0, 0);

	ArchManager::InitMultitasking();

	SetHostName("flux_host", 9);
	SetDomainName("flux_domain", 11);

	printk("Starting Init...\n");

#if BOOT == 1
    ArchManager::StartInit();
#endif
    
	while(1);
}
