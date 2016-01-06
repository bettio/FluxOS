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
 *   Name: archthreadsmanager.cpp                                          *
 *   Date: 15/11/2011                                                      *
 ***************************************************************************/

#include <task/archthreadsmanager.h>
#include <arch/mips/core/cpuregistersframe.h>

#include <cstdlib.h>
#include <cstring.h>
#include <stdint.h>

void *ArchThreadsManager::allocateKernelStack(void **stackAddr, int size)
{
    //size + stack overflow guards (4096 * 2)
    void *stack = 0;
    posix_memalign(&stack, 4096, size + 4096*2);
    memset(stack, 0, size + 4096*2); //This line is not only required to clean the stack, but also to get assured to use an allocated page
    *stackAddr = (void *) (((uint8_t *) (stack)) + 4096 + size);
    return stack;
}

ThreadControlBlock *ArchThreadsManager::createKernelThread(void (*fn)(), int flags, void *args)
{
    ThreadControlBlock *tmpCB = new ThreadControlBlock;
    RegistersFrame *tmpStack;
    tmpCB->stack = allocateKernelStack((void **) &tmpStack);
    tmpCB->currentStackPtr = (void *) (((unsigned long) tmpStack));
    tmpStack->pc = (uint32_t) fn;
    tmpStack->registers[28] = ((unsigned long) malloc(8192)) + 4096;

    return tmpCB;
}

//TODO: implement this
ThreadControlBlock *ArchThreadsManager::createUserThread(int flags)
{
    return 0;
}

//TODO: implement this
void ArchThreadsManager::makeExecutable(ThreadControlBlock *CB, void (*fn)(), int flags, void *args, int size)
{
}

//TODO: implement this
extern "C" void schedule()
{
}
