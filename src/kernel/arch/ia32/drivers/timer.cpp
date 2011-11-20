/***************************************************************************
 *   Copyright 2011 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 15/11/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/drivers/timer.h>

#include <core/systemtimer.h>
#include <arch/ia32/core/irq.h>
#include <arch/ia32/io.h>

#define BASE_COUNTER 0x40
#define PIT_CONTROL 0x43
#define PIT_FREQ 1193182
#define DIVIDER 11932
#define TICK_FREQ 100

void Timer::init()
{
    //0x43 - Timer control
    //0xC Chan 0 - LoHi Byte - Mode 0 - Binary
    ///outportb_p(0x43, 0xC); 

    //0x40 - Frequency divider for chan0 control
    ///outportb_p(0x40, DIVIDER & 0xFF);
    ///outportb_p(0x40, DIVIDER >> 8);

    SystemTimer::init(TICK_FREQ);
    IRQ::setHandler(SystemTimer::timerTickISR, 0);
    IRQ::enableIRQ(0);
}
