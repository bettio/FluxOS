/***************************************************************************
 *   Copyright 2016 by Davide Bettio <davide@uninstall.it>                 *
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
 *   Name: timeuapi.cpp                                                    *
 *   Date: 2/01/2016                                                       *
 ***************************************************************************/

#include <uapi/timeuapi.h>

#include <core/systemtimer.h>
#include <mm/usermemoryops.h>
#include <task/scheduler.h>

long TimeUAPI::time(long *t)
{
    long tSeconds = SystemTimer::time();
    if (t){
        int ret = putToUser(tSeconds, t);
        if (UNLIKELY(ret < 0)) {
            return ret;
        }
    }
    return tSeconds;
}

int TimeUAPI::stime(long *t)
{
    if (UNLIKELY(Scheduler::currentThread()->parentProcess->uid != 0)) {
        return -EPERM;
    }

    long time;
    int ret = getFromUser(&time, t);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }
    SystemTimer::setTime(time * 1000);

    return 0;
}
