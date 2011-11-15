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
 *   Name: archthreadsmanager.h                                            *
 *   Date: 15/11/2011                                                      *
 ***************************************************************************/

#ifndef _ARCHTHREADSMANAGER_H
#define _ARCHTHREADSMANAGER_H

#include <task/threadcontrolblock.h>

#define DEFAULT_KERNEL_SIZE 8192

class ArchThreadsManager
{
    public:
        static void *allocateKernelStack(void **stackAddr, int size = DEFAULT_KERNEL_SIZE);
        static ThreadControlBlock *createKernelThread(void (*fn)(), int flags = 0, void *args = 0);
};

#endif
 
