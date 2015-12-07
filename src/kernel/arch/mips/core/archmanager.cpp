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
 *   Name: archmanager.cpp                                                 *
 *   Date: 13/11/2015                                                      *
 ***************************************************************************/

#include <core/archmanager.h>
#include <core/printk.h>

#include <arch/mips/drivers/serial.h>
#include <arch/mips/mm/pagingmanager.h>
#include <arch/mips/core/exceptionsvector.h>
#include <core/syscallsmanager.h>
#include <task/userprocessimage.h>

#include <drivers/vt.h>
#include <task/task.h>

void ArchManager::Init()
{
    Serial::init();
    Out = Serial::Device();
}

void ArchManager::InitArch()
{
    ExceptionsVector::init();
    Task::init();
}

void ArchManager::InitMemoryManagment()
{
    PagingManager::init();
}

void ArchManager::InitMultitasking()
{
    SyscallsManager::init();
}

void ArchManager::InitHardware()
{
//    Vt::ReInit();
    Serial::reinit();
}

void ArchManager::StartInit()
{
    UserProcessImage::setupInitProcessImage();
}

void ArchManager::halt()
{
    asm(
        "waitForever:\n"
        "wait\n"
        "b waitForever\n"
    );
    while (1);
}

//TODO: please, move away this stuff
unsigned long kernel_heap_free_pos = KERNEL_HEAP_START;
