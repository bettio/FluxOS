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
 *   Name: systemtimer.cpp                                                 *
 *   Date: 19/11/2011                                                      *
 ***************************************************************************/

#include <core/systemtimer.h>
#include <task/scheduler.h>
#include <uapi/time.h>

long SystemTimer::startupTimestamp;
uint64_t SystemTimer::systemTicks;
QList<ThreadTimer> *SystemTimer::timers;
int SystemTimer::tickFrequency;

struct ThreadControlBlock;

void SystemTimer::init(int frequency)
{
    timers = new QList<ThreadTimer>();
    tickFrequency = frequency;
}

void SystemTimer::sleep(int millis, ThreadControlBlock *thread)
{
    thread->status = IWaiting;

    ThreadTimer timer;
    timer.parentThread = thread;
    timer.expiralSystemTime = systemTicks + (millis * tickFrequency) / 1000;
    timers->append(timer);

    //TODO: schedule the thread, please
}

int SystemTimer::sleep(struct timespec *duration, struct timespec *remaining)
{
    ThreadControlBlock *thread = Scheduler::currentThread();
    thread->status = IWaiting;

    ThreadTimer timer;
    timer.parentThread = thread;
    timer.expiralSystemTime = systemTicks + duration->tv_sec * tickFrequency + duration->tv_nsec * tickFrequency / 1000000000;
    timers->append(timer);

    //schedule will run some other thread and this will sleep until timer expires
    schedule();

    //thread is running again, probably timer has expired but it might have been interrupted
    //it might have been also scheduled for any other reason and the timer might be still alive
    for (int i = 0; i < timers->count(); i++) {
        if (timers->at(i).parentThread == thread) {
            int64_t remainingTicks = timers->at(i).expiralSystemTime - systemTicks;
            remaining->tv_sec = remainingTicks / tickFrequency;
            remaining->tv_nsec = 0;
            return (remainingTicks > 0) ? -EINTR : 0;
        }
    }
    remaining->tv_sec = 0;
    remaining->tv_nsec = 0;

    return 0;
}

void SystemTimer::timerTickISR()
{
    systemTicks++;
    
    //TODO: replace this with a priority queue, use the lowest expiralSystemTime value
    for (int i = 0; i < timers->count(); i++){
        if (timers->at(i).expiralSystemTime < systemTicks){
            timers->at(i).parentThread->status = Running;
            timers->removeAt(i);
            //TODO: schedule the thread, please
        }
    }
}

uint64_t SystemTimer::time()
{
    return (startupTimestamp + (systemTicks / tickFrequency)) * 1000; 
}

void SystemTimer::setTime(uint64_t time)
{
    startupTimestamp = (time / 1000) - systemTicks / tickFrequency;
}
