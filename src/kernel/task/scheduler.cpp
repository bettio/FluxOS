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
 *   Name: scheduler.cpp                                                   *
 *   Date: 15/11/2011                                                      *
 ***************************************************************************/

#include <task/scheduler.h>

#include <stdint.h>
#include <cstdlib.h>
#include <task/eventsmanager.h>

QList<ThreadControlBlock *> *Scheduler::threads;
int Scheduler::s_currentThread = 0;
volatile bool Scheduler::preemptionEnabled = true;

void Scheduler::init()
{
    threads = new QList<ThreadControlBlock *>();
    EventsManager::init();
}

ThreadControlBlock *Scheduler::nextThread()
{
    ThreadControlBlock *tB;
    do {
        s_currentThread = (s_currentThread + 1) % threads->size();
        tB = threads->at(s_currentThread);
    } while (tB->status != Running);
    return tB;
}

ThreadControlBlock *Scheduler::currentThread()
{    
    return threads->at(s_currentThread);
}

void Scheduler::waitForEvents()
{

}

void Scheduler::inhibitPreemption()
{
    preemptionEnabled = false;
}

bool Scheduler::isPreemptionInhibited()
{
    return !preemptionEnabled;
}

void Scheduler::restorePreemption()
{
    preemptionEnabled = true;
}
