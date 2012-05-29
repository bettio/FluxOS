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

/*
 * SP 804 timer driver
 */

#include <arch/arm/drivers/timer.h>

#include <core/systemtimer.h>
#include <arch/arm/drivers/vectoredinterruptcontroller.h>
#include <core/printk.h>
#include <stdint.h>

#define TICK_FREQ 100

void Timer::init()
{
    *((volatile uint32_t *) 0x101E2008) = 0x80 | 0x20; //enable timer
    *((volatile uint32_t *) 0x101E2000) = 1000000 / TICK_FREQ;

    SystemTimer::init(TICK_FREQ);
    VectoredInterruptController::registerAndEnableInterrupt(4, interruptHandler, VectoredInterruptController::IRQ);
}

void Timer::interruptHandler()
{
    SystemTimer::timerTickISR();
    *((volatile uint32_t *) 0x101E200C) = 0; //interrupt clear
}
