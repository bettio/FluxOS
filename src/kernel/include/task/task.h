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

#include <task/taskdescriptor.h>
#include <arch.h>

class Task{
	public:
		static TaskDescriptor *CurrentTask();
		static TaskDescriptor TaskDescriptorTable[256];
		static void SetCurrentTask(int ctask);
		static int CurrentTaskPid();
		static int SetUid(unsigned int uid);
		static int SetGid(unsigned int gid);
		static int NewPID();
		static bool TaskName(int pid, char * dest, int n);
		static int MaxUsedTaskPid();
		static int CreateNewTask(const char *name, bool user);
	private:
		static int current_task;
		static int m_MaxUsedTaskPid;
};

#endif
