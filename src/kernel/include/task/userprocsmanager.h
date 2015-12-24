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
 *   Name: userprocsmanager.h                                              *
 *   Date: 13/11/2015                                                      *
 ***************************************************************************/

#ifndef _USER_PROCS_MANAGER_H
#define _USER_PROCS_MANAGER_H

struct RegistersFrame;
struct ThreadControlBlock;

class UserProcsManager
{
    public:
        static void setupStackAndRegisters(RegistersFrame *frame, void *entryPoint, void *userSpaceStack, unsigned long userStackSize,
                                           int argc, int argsSize, int envc, int envSize, int auxc, int auxSize,
                                           char **argsList[], char **argsBlock,
                                           char **envList[], char **envBlock,
                                           char **auxList[], char **auxBlock);
        static int fork(void *stack);
        static void *createUserProcessStack(unsigned int size);
        static void startRegsFrame(RegistersFrame *frame);
        static void makeUserThread(ThreadControlBlock *thread);
        static RegistersFrame *createNewRegistersFrame();
};

#endif

