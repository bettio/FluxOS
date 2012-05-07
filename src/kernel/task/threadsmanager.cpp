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
 *   Name: threadsmanager.cpp                                              *
 *   Date: 15/11/2011                                                      *
 ***************************************************************************/

#include <task/threadsmanager.h>

#include <task/scheduler.h>
#include <task/archthreadsmanager.h>

void ThreadsManager::createKernelThread(void (*fn)(), int flags, void *args)
{
    ThreadControlBlock *tCB = ArchThreadsManager::createKernelThread(fn, flags, args);
    Scheduler::threads->append(tCB);  
}

ThreadControlBlock *ThreadsManager::createUserThread(int flags)
{
    ThreadControlBlock *tCB = ArchThreadsManager::createUserThread(flags);
    Scheduler::threads->append(tCB);  
    return tCB;
}

void ThreadsManager::makeExecutable(ThreadControlBlock *tCB, void (*fn)(), int flags, void *args)
{
    ArchThreadsManager::makeExecutable(tCB, fn, flags, args);
}
