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

#include <arch/ia32/core/registersframe.h>
#include <arch/ia32/mm/pagingmanager.h>
#include <cstdlib.h>
#include <cstring.h>
#include <stdint.h>

/**
 * @return a pointer to the the stack memory area, useful to free() it. Don't use it for the stack.
 */
void *ArchThreadsManager::allocateKernelStack(void **stackAddr, int size)
{
    //size + stack overflow guards (4096 * 2)
    void *stack = 0;
    posix_memalign(&stack, PAGE_BOUNDARY, size + 4096*2);
    memset(stack, 0, size + 4096*2); //This line is not only required to clean the stack, but also to get assured to use an allocated page
    *stackAddr = (void *) (((uint8_t *) (stack)) + 4096 + size);
    return stack;
}

ThreadControlBlock *ArchThreadsManager::createKernelThread(void (*fn)(), int flags, void *args)
{
    ThreadControlBlock *tmpCB = new ThreadControlBlock;
    RegistersFrame *tmpStack;
    tmpCB->stack = allocateKernelStack((void **) &tmpStack);
    tmpCB->currentStackPtr = (char *) tmpStack;
    tmpStack->eip = (uint32_t) fn;
    tmpStack->cs = 8;
    tmpStack->eflags = 0x202;

    return tmpCB;
}

ThreadControlBlock *ArchThreadsManager::createUserThread(int flags)
{
    ThreadControlBlock *tmpCB = new ThreadControlBlock;
    void *tmpStack;
    tmpCB->stack = allocateKernelStack((void **) &tmpStack);
    tmpCB->kernelStack = tmpStack;

    return tmpCB;
}

void ArchThreadsManager::makeExecutable(ThreadControlBlock *CB, void (*fn)(), int flags, void *args, int size)
{
    void *tmpStack;
    CB->stack2 = allocateKernelStack((void **) &tmpStack);
    CB->currentStackPtr = (char *) tmpStack - size - sizeof(RegistersFrame);
    RegistersFrame *registers = (RegistersFrame *) CB->currentStackPtr;
    registers->eip = (uint32_t) fn;
    registers->cs = 8;
    registers->eflags = 0x202;
    memcpy(registers + 1, args, size);
}

