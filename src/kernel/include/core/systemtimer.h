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
 *   Name: systemtimer.h                                                   *
 *   Date: 19/11/2011                                                      *
 ***************************************************************************/

#ifndef _SYSTEMTIMER_H_
#define _SYSTEMTIMER_H_

#include <stdint.h>
#include <QList>
#include <task/threadcontrolblock.h>

struct ThreadTimer
{
    ThreadControlBlock *parentThread;
    unsigned long expiralSystemTime; //should I use ticks_t
};


class SystemTimer
{
    public:
        static void init(int frequency);
        static void timerTickISR();
        static void sleep(int millis, ThreadControlBlock *thread);
        
    private:
        static uint64_t systemTicks;
        static QList<ThreadTimer> *timers;
        static int tickFrequency;
};

#endif
