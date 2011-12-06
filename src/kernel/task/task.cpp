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
#include <task/scheduler.h>
#include <filesystem/vnodemanager.h>
#include <arch.h>
#include <ListWithHoles>

ListWithHoles<ProcessControlBlock *> *Task::processes;

void Task::init()
{
    processes = new ListWithHoles<ProcessControlBlock *>();
}

int Task::SetUid(unsigned int uid)
{
	ProcessControlBlock *task = Scheduler::currentThread()->parentProcess;

	if (task->uid == 0){
		task->uid = uid;

		return 0;
	}else{
		return -EPERM;
	}
}

int Task::SetGid(unsigned int gid)
{
	ProcessControlBlock *task = Scheduler::currentThread()->parentProcess;

	if (task->uid == 0){
		task->gid = gid;

		return 0;
	}else{
		return -EPERM;
	}
}

ProcessControlBlock *Task::CreateNewTask(const char *name)
{
	ProcessControlBlock *process = new ProcessControlBlock;
        int newTaskPid = processes->add(process);
	process->pid = newTaskPid;
        process->uid = 0;
	process->gid = 0;
	process->name = strdup(name);
        process->parent = 0;
	process->dataSegmentEnd = (void *) 0xC0000000;
        process->openFiles = new ListWithHoles<FileDescriptor *>();
	VNode *ttyNode;
        FileSystem::VFS::RelativePathToVnode(0, "/dev/tty1", &ttyNode, true);
	//stdin
        FileDescriptor *fdesc = new FileDescriptor(ttyNode);
        process->openFiles->add(fdesc);
	//stdout
        fdesc = new FileDescriptor(FileSystem::VNodeManager::ReferenceVnode(ttyNode));
        process->openFiles->add(fdesc);
	//stderr
	fdesc = new FileDescriptor(FileSystem::VNodeManager::ReferenceVnode(ttyNode));
	process->openFiles->add(fdesc);

        VNode *cwdNode;
	FileSystem::VFS::RelativePathToVnode(0, "/", &cwdNode);
	process->currentWorkingDirNode = cwdNode;

        process->status = READY;

	return process;
}

ProcessControlBlock *Task::NewProcess(const char *name)
{
    ProcessControlBlock *parent = Scheduler::currentThread()->parentProcess;

    ProcessControlBlock *process = new ProcessControlBlock;
    int newTaskPid = processes->add(process);
    process->pid = newTaskPid;
    process->uid = parent->uid;
    process->gid = parent->gid;
    process->name = strdup(name);
    process->parent = parent;
    process->dataSegmentEnd = (void *) 0xC0000000;
    process->openFiles = new ListWithHoles<FileDescriptor *>();
    for (int i = 0; i < parent->openFiles->size(); i++){
        FileDescriptor *oldFd = parent->openFiles->at(i);
	if (oldFd != 0){
            FileDescriptor *newFd = new FileDescriptor(FileSystem::VNodeManager::ReferenceVnode(oldFd->node));
            newFd->fpos = oldFd->fpos;
            process->openFiles->add(newFd);
        }
    }

    process->currentWorkingDirNode = FileSystem::VNodeManager::ReferenceVnode(parent->currentWorkingDirNode);


    process->status = READY;

    return process;
}

