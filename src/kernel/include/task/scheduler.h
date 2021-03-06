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
 *   Name: scheduler.h                                                     *
 *   Date: 15/11/2011                                                      *
 ***************************************************************************/

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <QList>
#include <task/threadcontrolblock.h>

extern "C" void schedule();

class Scheduler
{
    public:
        static void init();
        static ThreadControlBlock *nextThread();
        static ThreadControlBlock *currentThread();
        static QList<ThreadControlBlock *> *threads;
        static void waitForEvents();
        static void inhibitPreemption();
        static bool isPreemptionInhibited();
        static void restorePreemption();

    private:
        static int s_currentThread;
        static volatile bool preemptionEnabled;
};

#endif
