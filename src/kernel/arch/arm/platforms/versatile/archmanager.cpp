/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/

#include <core/archmanager.h>
#include <core/printk.h>

#include <drivers/vt.h>
#include <drivers/fbconsole.h>
#include <arch/arm/drivers/serial.h>

#include <arch/arm/core/exceptionsvector.h>
#include <arch/arm/core/syscallsmanager.h>
#include <arch/arm/drivers/pl110fb.h>
#include <arch/arm/drivers/timer.h>
#include <arch/arm/drivers/vectoredinterruptcontroller.h>
#include <arch/arm/drivers/interruptcontroller.h>

void enableInterrupts();

void ArchManager::Init()
{
    Pl110Fb::init();
    //Serial::Init();  
    Out = Vt::Device();
}

void enableInterrupts()
{
    asm("mrs r0, cpsr\n"
        "bic r0,r0,#0x80\n"
        "msr cpsr_c,r0\n");
}

void ArchManager::InitArch()
{
    ExceptionsVector::init();
    VectoredInterruptController::init();
    InterruptController::init();
    SyscallsManager::init();
    enableInterrupts();
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
    //Serial::ReInit();
}

void ArchManager::StartInit()
{
}
