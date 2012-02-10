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

#include <arch/ia32/mm/pagingmanager.h>
#include <cstdlib.h>
#include <cstring.h>
#include <stdint.h>

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
};

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
    tmpCB->currentStackPtr = tmpStack;
    tmpStack->eip = (uint32_t) fn;
    tmpStack->cs = 8;
    tmpStack->eflags = 0x202;

    return tmpCB;
}
