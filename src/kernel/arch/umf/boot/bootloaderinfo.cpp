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
 *   Name: bootloaderinfo_multiboot1.cpp                                   *
 *   Date: 18/11/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/boot/multibootinfo.h>
#include <boot/bootloaderinfo.h>
#include <arch/ia32/boot/multiboot.h>
#include <defs.h>
#include <core/elf.h>

void *BootLoaderInfo::module(int i)
{
    return 0;
}

long unsigned int BootLoaderInfo::moduleSize(int)
{
    return 0;
}

int BootLoaderInfo::modulesCount()
{
    return 0;
}
