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
 *   Name: interruptcontroller.cpp                                         *
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/
 
#include <arch/arm/drivers/interruptcontroller.h>

#include <arch/arm/drivers/vectoredinterruptcontroller.h>
#include <core/printk.h>
#include <stdint.h>

#define ENABLE_READ *((volatile uint32_t *) 0x10003008)
#define ENABLE_SET *((volatile uint32_t *) 0x10003008)
#define ENABLE_CLEAR *((volatile uint32_t *) 0x1000300C)

void InterruptController::init()
{
    VectoredInterruptController::registerAndEnableInterrupt(31, handler, VectoredInterruptController::IRQ);
}

void InterruptController::registerAndEnableInterrupt(int intNum, void (*handler)())
{
    ENABLE_SET = ENABLE_CLEAR | (1 << intNum);
}

void InterruptController::handler()
{
    printk("Error: unknown IRQ/FIQ (handler)\n");
    while(1);
}
