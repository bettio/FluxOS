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
 *   Date: 05/09/2006                                                      *
 ***************************************************************************/

#include <core/printk.h>
#include <core/archmanager.h>
#include <drivers/vt.h>
#include <arch/ia32/core/idt.h>
#include <arch/ia32/drivers/video.h>
#include <arch/ia32/core/irq.h>
#include <arch/ia32/core/gdt.h>
#include <arch/ia32/core/pci.h>

void ArchManager::Init()
{
    GDT::init();

    Video::init();
    Out = Vt::Device();
}

void ArchManager::InitArch()
{
    IDT::init();
    IRQ::init();

    asm("sti");
    
    PCI::init();
}

void ArchManager::InitMemoryManagment()
{
}

void ArchManager::InitMultitasking()
{
}

void ArchManager::InitHardware()
{
}

void ArchManager::StartInit()
{
}

