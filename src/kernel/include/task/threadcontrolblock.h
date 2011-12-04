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
 *   Name: threadcontrolblock.h                                            *
 *   Date: 15/11/2011                                                      *
 ***************************************************************************/

#ifndef _THREADCONTROLBLOCK_H_
#define _THREADCONTROLBLOCK_H_

#include <task/processcontrolblock.h>

enum ThreadStatus
{
    Running, /* R */
    UWaiting, /* D */
    IWaiting, /* S */
    Defunct /* Z */
};

struct ThreadControlBlock
{
    void *stack;
    void *currentStackPtr;
    ProcessControlBlock *parentProcess;
    ThreadStatus status;
    unsigned int tid;

    void *addressSpaceTable;
};

#endif

