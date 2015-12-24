/***************************************************************************
 *   Copyright 2015 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: userprocsmanager.cpp                                            *
 *   Date: 09/11/2015                                                      *
 ***************************************************************************/

#include <task/userprocsmanager.h>
#include <arch/mips/core/cpuregistersframe.h>

#include <arch/mips/mm/pagingmanager.h>
#include <core/elfloader.h>
#include <task/archthreadsmanager.h>
#include <task/processcontrolblock.h>
#include <task/scheduler.h>
#include <task/task.h>
#include <task/threadcontrolblock.h>

//TODO: fix values here
#define USER_DEFAULT_STACK_ADDR 0x1000

static inline int padToWord(int addr)
{
    return (addr & 0xFFFFFFFC) + 4;
}

void UserProcsManager::setupStackAndRegisters(RegistersFrame *frame, void *entryPoint, void *userSpaceStack, unsigned long userStackSize,
                            int argc, int argsSize, int envc, int envSize, int auxc, int auxSize,
                            char **argsList[], char **argsBlock,
                            char **envList[], char **envBlock,
                            char **auxList[], char **auxBlock)
{
    unsigned long varsBlockSize = sizeof(unsigned long) +
                                  sizeof(char *) * (argc + 1) + sizeof(char *) * (envc + 1) + sizeof(void *) * 2 * auxc
                                  + padToWord(argsSize) + padToWord(envSize) + padToWord(auxSize);
    void *sp =  (void *) (((unsigned long) userSpaceStack) + userStackSize - varsBlockSize);
    frame->registers[REGISTERS_FRAME_SP_REGISTER] = (unsigned long) sp;
    frame->pc = (unsigned long) entryPoint;

    unsigned long *vars = (unsigned long *) sp;
    /*
     MIPS ABI, user process stack:
     - Lower address
     - argc <- SP
     - pointers array to args
     - 0
     - pointers array to env vars
     - 0
     - aux vector
     - 0
     - Args block (args array point to this)
     - env block (env vars array point to this)
     - Higher address
    */
    int pos = 0;

    vars[pos] = argc;
    pos++;

    *argsList = (char **) &vars[pos];
    pos += argc + 1;

    *envList = (char **) &vars[pos];
    pos += envc + 1;

    *auxList = (char **) &vars[pos];
    pos += (auxc + 1)*2;

    *argsBlock = (char *) &vars[pos];
    pos += (argsSize / 4) + 1;

    *envBlock = (char *) &vars[pos];
    pos += (envSize / 4) + 1;

    *auxBlock = (char *) &vars[pos];
}


RegistersFrame *UserProcsManager::createNewRegistersFrame()
{
    return new RegistersFrame;
}

void *UserProcsManager::createUserProcessStack(unsigned int size)
{
    return malloc(size);
}

void UserProcsManager::makeUserThread(ThreadControlBlock *thread)
{
}

//TODO: remove this
void UserProcsManager::startRegsFrame(RegistersFrame *frame)
{
    asm("add $sp, $0, %1\n"
        "jr %0\n"
        : : "r" (frame->pc),
            "r" (frame->registers[REGISTERS_FRAME_SP_REGISTER])
       );
}

//TODO: implement fork here
int UserProcsManager::fork(void *stack)
{
    return 0;
}
