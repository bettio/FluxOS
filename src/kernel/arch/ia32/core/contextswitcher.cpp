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

struct RegistersFrame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t null;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t fine;
};

void *kernelStackAlloc(void **stackAddr, int size = 8129);

void *kernelStackAlloc(void **stackAddr, int size)
{
    //size + stack overflow guards (4096 * 2)
    void *stack = malloc(size + 4096*2);
    *stackAddr = (void *) (((uint8_t *) (stack)) + 4096 + size);
    return stack;
}

void createKernelThread(void *functionPtr)
{
    ThreadControlBlock *tmpCB = new ThreadControlBlock;
    RegistersFrame *tmpStack;
    tmpCB->stack = kernelStackAlloc((void **) &tmpStack);
    tmpCB->currentStackPtr = tmpStack;
    tmpStack->eip = (uint32_t) functionPtr;
    tmpStack->cs = 8;
    tmpStack->eflags = 0x202;
    Scheduler::threads->append(tmpCB);  
}

void testA()
{
    while(1) *((unsigned char *) 0xB8000) = 'A';   
}

void testB()
{
    while(1) *((unsigned char *) 0xB8000) = 'B';   
}

void ContextSwitcher::init()
{
    Scheduler::init();
    
    //We must create at least one thread, this will be replaced by ContextSwitcher::schedule with the current
    //running thread so it can be restored.
    ThreadControlBlock *currentThread = new ThreadControlBlock;
    currentThread->stack = 0;
    currentThread->currentStackPtr = 0;
    Scheduler::threads->append(currentThread);
    
    createKernelThread((void *) testA);
    createKernelThread((void *) testB);
}


void ContextSwitcher::schedule(long *esp)
{
    Scheduler::currentThread()->currentStackPtr = (void *) *esp;
    *esp = (long) Scheduler::nextThread()->currentStackPtr;
}
