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
 *   Name: contextswitcher.cpp                                             *
 *   Date: 15/11/2011                                                      *
 ***************************************************************************/

#include <stdint.h>
#include <arch/ia32/core/contextswitcher.h>
#include <task/scheduler.h>
#include <cstdlib.h>

void ContextSwitcher::init()
{
    Scheduler::init();
    
    //We must create at least one thread, this will be replaced by ContextSwitcher::schedule with the current
    //running thread so it can be restored.
    ThreadControlBlock *currentThread = new ThreadControlBlock;
    currentThread->stack = 0;
    currentThread->currentStackPtr = 0;
    Scheduler::threads->append(currentThread);
}


void ContextSwitcher::schedule(long *esp)
{
    Scheduler::currentThread()->currentStackPtr = (void *) *esp;
    *esp = (long) Scheduler::nextThread()->currentStackPtr;
}