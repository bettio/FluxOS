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
 *   Name: diskimage.cpp                                                   *
 *   Date: 25/04/2005                                                      *
 ***************************************************************************/

#include <drivers/blockdevicemanager.h>
#include <arch/umf/drivers/diskimage.h>
#include <drivers/diskpart.h>
#include <cstdlib.h>

#include <arch/umf/core/hostsyscalls.h>

#include <core/printk.h>


int fdescs[3];

void DiskImage::Init()
{
	fdescs[0] = HostSysCalls::open("FluxFloppyDisk", 0, 0);

	if (fdescs[0] < 0){
		printk("Error while opening FluxFloppyDisk.\n");
	}else{
		BlockDevice *bd0 = new BlockDevice;
		//TODO: Warning: unchecked malloc
		bd0->int_cookie = 0;
		bd0->Major = 0;
		bd0->Minor = 0;
		bd0->name = "fd0";
		bd0->ReadBlock = ReadBlock;
		BlockDeviceManager::Register(bd0);
	}

	fdescs[1] = HostSysCalls::open("disk0", 0, 0);

	if (fdescs[1] < 0){
		printk("Error while opening disk0.\n");
	}else{
		BlockDevice *bd1 = new BlockDevice;
		//TODO: Warning: unchecked malloc
		bd1->int_cookie = 1;
		bd1->Major = 1;
        bd1->Minor = 0;
		bd1->name = "disk0";
		bd1->ReadBlock = ReadBlock;
		BlockDeviceManager::Register(bd1);
		DiskPart::CreatePartitionsDevices(bd1, "disk0p#");
	}

	fdescs[2] = HostSysCalls::open("disk1", 0, 0);

	if (fdescs[2] < 0){
		printk("Error while opening disk1.\n");
	}else{
		BlockDevice *bd2 = new BlockDevice;
		//TODO: Warning: unchecked malloc
		bd2->int_cookie = 2;
		bd2->Major = 2;
        bd2->Minor = 0;
		bd2->name = "disk1";
		bd2->ReadBlock = ReadBlock;
		BlockDeviceManager::Register(bd2);
		DiskPart::CreatePartitionsDevices(bd2, "disk1p#");
	}
}

void DiskImage::ReadBlock(BlockDevice *bd, int block, int blockn, uint8_t *blockbuffer)
{
	HostSysCalls::lseek(fdescs[bd->int_cookie], block*512, 0);
	HostSysCalls::read(fdescs[bd->int_cookie], (void *) blockbuffer, blockn*512);
}
