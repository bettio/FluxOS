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
 *   Name: timer.cpp                                                       *
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/

#include <arch/arm/drivers/timer.h>

#include <arch/arm/drivers/vectoredinterruptcontroller.h>
#include <core/printk.h>
#include <stdint.h>

void Timer::init()
{
    //Timer stuff
    *((volatile uint32_t *) 0x101E2008) = 0x84 | 0x20; //enable timer
    *((volatile uint32_t *) 0x101E2000) = 0xFF;
    *((volatile uint32_t *) 0x101E2004) = 0x10;
    
    VectoredInterruptController::registerAndEnableInterrupt(4, interruptHandler, VectoredInterruptController::IRQ);
}

void Timer::interruptHandler()
{
    *((volatile uint32_t *) 0x101E200C) = 0; //interrupt clear
}
