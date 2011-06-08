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
 *   Name: vectoredinterruptcontroller.cpp                                 *
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/

#include <arch/arm/drivers/vectoredinterruptcontroller.h>

#include <arch/arm/core/exceptionsvector.h>
#include <core/printk.h>
#include <stdint.h>

extern "C" void irqHandler();
extern "C" void fiqHandler();
extern "C" void defaultHandler();

void VectoredInterruptController::init()
{
    ExceptionsVector::setHandler(ExceptionsVector::IRQ, (void *) irqHandler);
    ExceptionsVector::setHandler(ExceptionsVector::FIQ, (void *) fiqHandler);
    
    *((volatile uint32_t *) (0x10140034)) = (uint32_t) defaultHandler; //default vect addr
}

void VectoredInterruptController::registerAndEnableInterrupt(int intNum, void (*handler)(), InterruptType type)
{
    *((volatile uint32_t *) 0x10140014) = 1 << intNum; //clear it before to enable it
    *((volatile uint32_t *) (0x10140114)) = (uint32_t) handler;
    *((volatile uint32_t *) (0x10140210)) = intNum | 0x20;
    *((volatile uint32_t *) 0x1014000C) = (type == FIQ) << intNum; //enable fiq
    *((volatile uint32_t *) 0x10140010) = 1 << intNum; //enable interrupt   
}

extern "C" void defaultHandler()
{
    printk("Error: unknown IRQ/FIQ\n");
    while(1);
}
