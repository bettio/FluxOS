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
 *   Name: memcalls.cpp                                                    *
 *   Date: 02/12/2012                                                      *
 ***************************************************************************/

#include <mm/memcalls.h>

#include <mm/memorycontext.h>
#include <task/scheduler.h>

//IMPLEMENT CHECKS
void *brk(void *ptr)
{
    ProcessControlBlock *process = Scheduler::currentThread()->parentProcess;

    if (ptr != NULL){
        if (ptr > process->dataSegmentEnd) {
            process->dataSegmentEnd = (void *) ((unsigned long) process->dataSegmentStart +
                                                (unsigned long) process->memoryContext->growExtent(process->dataSegmentStart, (unsigned long) ptr - (unsigned long) process->dataSegmentEnd));
        } else {
            printk("Error cannot reduce data segment\n");
        }
    }

    return process->dataSegmentEnd;
}

