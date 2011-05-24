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
 *   Name: archmanager.cpp                                                 *
 *   Date: 06/08/2006                                                      *
 ***************************************************************************/

#include <core/archmanager.h>
#include <core/printk.h>
#include <cstdlib.h>
#include <arch/umf/drivers/console.h>
#include <arch/umf/drivers/diskimage.h>
#include <arch/umf/linux-x86_64/syscall.h>

void UMMStartInit();

void ArchManager::Init()
{
	Out = Console::Device();
}


void ArchManager::InitArch()
{

}

void ArchManager::InitMemoryManagment()
{
	initmem();
}

void ArchManager::InitMultitasking()
{
	SysCall::Init();
}

void ArchManager::InitHardware()
{
	Console::ReInit();
	DiskImage::Init();
}

void ArchManager::StartInit()
{
    UMMStartInit();
}
