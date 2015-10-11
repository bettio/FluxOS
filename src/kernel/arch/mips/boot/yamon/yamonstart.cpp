/***************************************************************************
 *   Copyright 2015 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: yamonstart.cpp                                                  *
 *   Date: 13/11/2015                                                      *
 ***************************************************************************/

#include <arch/mips/boot/bootinfo.h>

#include <main.h>

int yamonKernelArgsNum;
void *yamonKernelArgsTable;
void *yamonEnvVarsTable;
unsigned long yamonRamSize;

extern "C" void yamonStart(int kernelArgsNum, char *kernelArgsTable, char *envVarsTable, unsigned long ramSize)
{
    yamonKernelArgsNum = kernelArgsNum;
    yamonKernelArgsTable = kernelArgsTable;
    yamonEnvVarsTable = envVarsTable;
    yamonRamSize = ramSize;

    main();
}

unsigned long BootInfo::physicalMemorySize()
{
    return yamonRamSize;
}

const char **BootInfo::kernelArgs()
{
    return (const char **) yamonKernelArgsTable;
}

const char **BootInfo::env()
{
    return (const char **) yamonEnvVarsTable;
}

const void *BootInfo::ramdisk()
{
    //FIXME: Ugly hack here
    return (const void *) 0x80410000;
}
