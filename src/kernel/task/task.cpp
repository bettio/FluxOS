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
 *   Name: task.cpp                                                        *
 ***************************************************************************/

#include <task/task.h>
#include <arch.h>

#include <lib/koof/intkeymap.h>

#ifdef ARCH_IA32_UMM_LINUX
#include <arch/umf/core/hostsyscalls.h>
extern IntKeyMap<int> *Pids;
#endif

int Task::m_MaxUsedTaskPid;

#warning 256 tasks maximum
TaskDescriptor Task::TaskDescriptorTable[256];
int Task::current_task;


int Task::SetUid(unsigned int uid)
{
	TaskDescriptor *task = Task::CurrentTask();

	if (task->Uid == 0){
		task->Uid = uid;

		return 0;
	}else{
		return -EPERM;
	}
}

int Task::SetGid(unsigned int gid)
{
	TaskDescriptor *task = Task::CurrentTask();

	if (task->Uid == 0){
		task->Gid = gid;

		return 0;
	}else{
		return -EPERM;
	}
}

bool Task::TaskName(int pid, char * dest, int n)
{
	if (pid >= m_MaxUsedTaskPid) return false;

	strncpy(dest, TaskDescriptorTable[pid].TaskName, n);

	return true;
}

int Task::NewPID(){
	static int FreePID = -1;
	
	FreePID++;
	m_MaxUsedTaskPid++;

	return FreePID;
}

TaskDescriptor *Task::CurrentTask()
{
        if (Pids != 0){
            return &TaskDescriptorTable[(*Pids)[HostSysCalls::getpid()]];
        }else{
            return &TaskDescriptorTable[current_task];
        }
}

int Task::MaxUsedTaskPid(){
	return m_MaxUsedTaskPid;
}

void Task::SetCurrentTask(int ctask)
{
	current_task = ctask;
}

int Task::CurrentTaskPid()
{
        return (*Pids)[HostSysCalls::getpid()];
}

int Task::CreateNewTask(const char *name, bool user)
{
	int newTaskPid = NewPID();

	strncpy(Task::TaskDescriptorTable[newTaskPid].TaskName, name, 64);

	TaskDescriptorTable[newTaskPid].Pid = newTaskPid;
	TaskDescriptorTable[newTaskPid].Parent = &Task::TaskDescriptorTable[newTaskPid];
	TaskDescriptorTable[newTaskPid].User = user;

	//TODO: Add stdin and stdout
	TaskDescriptorTable[newTaskPid].OpenFiles = new ListWithHoles<FileDescriptor *>();
	VNode *node;
        FileSystem::VFS::RelativePathToVnode(0, "/dev/tty1", &node, true);
        FileDescriptor *fdesc = new FileDescriptor(node);
        TaskDescriptorTable[newTaskPid].OpenFiles->add(fdesc);
        fdesc = new FileDescriptor(node);
        TaskDescriptorTable[newTaskPid].OpenFiles->add(fdesc);

	FileSystem::VFS::RelativePathToVnode(0, "/", &node);

	TaskDescriptorTable[newTaskPid].CwdNode = node;

	TaskDescriptorTable[newTaskPid].Status = READY;

	return newTaskPid;
}
