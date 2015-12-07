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

#include <task/task.h>
#include <core/printk.h>
#include <core/archmanager.h>
#include <core/syscallsmanager.h>
#include <drivers/vt.h>
#include <arch/ia32/core/idt.h>
#include <arch/ia32/drivers/ata.h>
#include <arch/ia32/drivers/keyboard.h>
#include <arch/ia32/drivers/ps2mouse.h>
#include <arch/ia32/drivers/timer.h>
#include <arch/ia32/drivers/video.h>
#include <arch/ia32/drivers/vesafb.h>
#include <arch/ia32/core/irq.h>
#include <arch/ia32/core/gdt.h>
#include <arch/ia32/core/pci.h>
#include <arch/ia32/core/userprocsmanager.h>
#include <arch/ia32/boot/multiboot.h>
#include <arch/ia32/boot/multibootinfo.h>
#include <boot/bootloaderinfo.h>
#include <arch/ia32/mm/pagingmanager.h>

#define ENABLE_VESAFB 1
#define ENABLE_EXPERIMENTAL_DRIVERS 0

void ArchManager::Init()
{
    GDT::init();

#if ENABLE_VESAFB
    if (MultiBootInfo::infoBlock->flags & MULTIBOOT_INFO_VIDEO_INFO){
        VesaFB::init(MultiBootInfo::infoBlock->vbe_mode_info);
    }else{
#endif
        Video::init();
#if ENABLE_VESAFB
    }
#endif
    Out = Vt::Device();
}

void ArchManager::InitArch()
{
    IDT::init();
    #ifndef NO_MMU
        PagingManager::init();
        #if ENABLE_VESAFB
        VesaFB::mapPhysicalMem();
        #endif
    #endif
    Task::init();
    IRQ::init();
    SyscallsManager::init();
    
    PCI::init();

    asm("sti");
    
    Timer::init();
}

void ArchManager::InitMemoryManagment()
{
}

void ArchManager::InitMultitasking()
{
}

void ArchManager::InitHardware()
{
    Vt::ReInit();
    Keyboard::init();
#if ENABLE_EXPERIMENTAL_DRIVERS
    ATA::init();
    PS2Mouse::init();
#endif
#if ENABLE_VESAFB
    VesaFB::registerDevice();
#endif
}

void ArchManager::StartInit()
{
    UserProcsManager::createInitProcess();
}

//try to triple fault to reset the CPU
void ArchManager::reboot()
{
    char c;
    unsigned long int idtReg[2];
    idtReg[0] = 0;
    idtReg[1] = (unsigned long int) &c;
    asm volatile ("lidt (%0)": :"g" ((char *) idtReg + 2));
    //this should work, but it causes a null point error on qem: asm volatile ("lidt 0"); 
    asm("int $0x80");
    while (1);   
}

void ArchManager::halt()
{
    asm(
        "waitForever:\n"
        "hlt\n"
        "jmp waitForever\n"
    );
    while (1);
}
