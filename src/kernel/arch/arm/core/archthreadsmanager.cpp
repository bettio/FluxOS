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

#include <cstdlib.h>
#include <cstring.h>
#include <stdint.h>

struct RegistersFrame
{
    uint32_t spsr;
    uint32_t _sp;
    uint32_t _lr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t pc;
};

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
    tmpCB->currentStackPtr = tmpStack;
    tmpStack->pc = (uint32_t) fn;
    tmpStack->spsr = 0x1F;
    tmpStack->_sp = ((uint32_t) malloc(8192)) + 4096;

    return tmpCB;
}

ThreadControlBlock *ArchThreadsManager::createUserThread(int flags)
{
    return 0;
}

void ArchThreadsManager::makeExecutable(ThreadControlBlock *CB, void (*fn)(), int flags, void *args)
{
}

