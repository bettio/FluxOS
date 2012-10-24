/***************************************************************************
 *   Copyright 2007 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: task.h                                                          *
 ***************************************************************************/

#ifndef _TASK_TASK_H
#define _TASK_TASK_H

#include <task/processcontrolblock.h>
#include <ListWithHoles>
#include <arch.h>

class Task{
	public:
        static void init();
        static int SetUid(unsigned int uid);
        static int SetGid(unsigned int gid);
        static ProcessControlBlock *CreateNewTask(const char *name);
        static ProcessControlBlock *NewProcess(const char *name);
        static ListWithHoles<ProcessControlBlock *> *processes;
        static void closeAllFiles(ProcessControlBlock *process);
        static void exit(int exitStatus);
        static int waitpid(int pid, int *status, int options);
        static void notify(ProcessControlBlock *p);
};

#endif
