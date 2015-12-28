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

#include <mm/memorycontext.h>
#include <task/scheduler.h>
#include <filesystem/vnodemanager.h>
#include <filesystem/fscalls.h>
#include <arch.h>
#include <ListWithHoles>
#include <core/systemerrors.h>

#define ENABLE_DEBUG_MSG 0
#include <debugmacros.h>

QHash<int, ProcessControlBlock *> *Task::processes;
int Task::lastUsedPID;

bool Task::ProcessIterator::operator!=(const ProcessIterator other) const
{
    return other.hIt != hIt;
}

bool Task::ProcessIterator::operator==(const ProcessIterator other) const
{
    return other.hIt == hIt;
}

Task::ProcessIterator &Task::ProcessIterator::operator++()
{
    ++hIt;
    return *this;
}

void Task::init()
{
    processes = new QHash<int, ProcessControlBlock *>();
    if (IS_NULL_PTR(processes)) {
        kernelPanic("failed to init processes hash table.");
    }
}

bool Task::isValidPID(int pid)
{
    return processes->value(pid) != NULL;
}

Task::ProcessIterator Task::processEnumerationBegin()
{
    ProcessIterator it;
    it.hIt = processes->constBegin();
    return it;
}

Task::ProcessIterator Task::processEnumerationEnd()
{
    ProcessIterator it;
    it.hIt = processes->constEnd();
    return it;
}

ProcessControlBlock *Task::CreateNewTask()
{
    DEBUG_MSG("Task::CreateNewTask(%s)\n");

    lastUsedPID++;

	ProcessControlBlock *process = new ProcessControlBlock;
        processes->insert(lastUsedPID, process);
	process->pid = lastUsedPID;
        process->uid = 0;
	process->gid = 0;
        process->parent = 0;
        process->memoryContext = new MemoryContext();
        process->dataSegmentStart = NULL;
        process->openFiles = new ListWithHoles<FileDescriptor *>();
	VNode *ttyNode;
        int res = FileSystem::VFS::RelativePathToVnode(0, "/dev/tty1", &ttyNode, true);
        if (res < 0){
            printk("Cannot find any /dev/tty1 for stdin/stdout/stderr: the process will not be created\n");
            return 0;
        }

	//stdin
    FileDescriptor *fdesc = new FileDescriptor(ttyNode);
    fdesc->flags = O_RDONLY;
    process->openFiles->add(fdesc);

    //stdout
    fdesc = new FileDescriptor(FileSystem::VNodeManager::ReferenceVnode(ttyNode));
    fdesc->flags = O_WRONLY;
    process->openFiles->add(fdesc);

    //stderr
    fdesc = new FileDescriptor(FileSystem::VNodeManager::ReferenceVnode(ttyNode));
    fdesc->flags = O_WRONLY;
    process->openFiles->add(fdesc);

    VNode *cwdNode;
    res = FileSystem::VFS::RelativePathToVnode(0, "/", &cwdNode);
    if (res < 0){
        printk("Cannot find any root filesystem\n");
        return 0;
    }
	    
    process->currentWorkingDirNode = cwdNode;
    process->umask = 0;
    process->cmdline = NULL;
    process->cmdlineSize = 0;
    process->status = READY;

	return process;
}

ProcessControlBlock *Task::NewProcess()
{
    DEBUG_MSG("Task::CreateNewTask(%s)\n");

    lastUsedPID++;

    ProcessControlBlock *parent = Scheduler::currentThread()->parentProcess;

    ProcessControlBlock *process = new ProcessControlBlock;
    processes->insert(lastUsedPID, process);
    process->pid = lastUsedPID;
    process->uid = parent->uid;
    process->gid = parent->gid;
    process->parent = parent;
    process->memoryContext = new MemoryContext();
    process->dataSegmentStart = NULL;
    process->openFiles = new ListWithHoles<FileDescriptor *>();
    for (int i = 0; i < parent->openFiles->size(); i++){
        FileDescriptor *oldFd = parent->openFiles->at(i);
	    if (oldFd != 0){
            FileDescriptor *newFd = new FileDescriptor(FileSystem::VNodeManager::ReferenceVnode(oldFd->node));
            newFd->fpos = oldFd->fpos;
            newFd->flags = oldFd->flags;
            process->openFiles->add(newFd);
        }
    }

    process->currentWorkingDirNode = FileSystem::VNodeManager::ReferenceVnode(parent->currentWorkingDirNode);
    process->umask = parent->umask;
    process->cmdline = NULL;
    process->cmdlineSize = 0;
    process->status = READY;

    return process;
}

void Task::closeAllFiles(ProcessControlBlock *process)
{
    for (int i = 0; i < process->openFiles->size(); i++){
	    if (process->openFiles->at(i) != NULL){
            close(i);
        }
    }
}

void Task::exit(int exitStatus)
{
    terminateProcess(Scheduler::currentThread(), exitStatus);
    while (1);
}

int Task::waitpid(int pid, int *status, int options)
{
    ProcessControlBlock *p;

    if (pid != -1){
        p = Task::processes->value(pid);
        if (p == NULL || p->parent != Scheduler::currentThread()->parentProcess) {
            return -ECHILD;
        }

        while (p->status != TERMINATED) Scheduler::waitForEvents();
    
    }else{
        while (1){
            for (Task::ProcessIterator it = Task::processEnumerationBegin(); it != Task::processEnumerationEnd(); ++it) {
                p = it.process();
                if ((p->status == TERMINATED) && (p->parent == Scheduler::currentThread()->parentProcess)) break;
            }
            if ((p->status == TERMINATED) && (p->parent == Scheduler::currentThread()->parentProcess)){
                pid = p->pid;
                break;
            }
            Scheduler::waitForEvents();
        }
    }

    *status = p->exitStatus;

    Task::processes->remove(pid);
    delete p->openFiles;
    delete p;

    return pid;
}

int Task::terminateProcess(ThreadControlBlock *thread, int exitStatus)
{
    ProcessControlBlock *process = thread->parentProcess;
    closeAllFiles(process);
    FileSystem::VNodeManager::PutVnode(process->currentWorkingDirNode);
    process->exitStatus = exitStatus;
    process->status = TERMINATED;
    notify(process->parent);
    thread->status = UWaiting;
    
    return 0;
}

int Task::kill(int pid, int signal)
{
    ProcessControlBlock *target = Task::processes->value(pid);
    if (target == NULL) return -ESRCH;

    ProcessControlBlock *currentProcess = Scheduler::currentThread()->parentProcess;
    if (currentProcess->uid != ROOT_UID){
        if (currentProcess->uid != target->uid) return -EPERM;
    }

    switch (signal){
        case SIGKILL:
            //TODO: kill all threads
            terminateProcess(target->mainThread, -1);
            return 0;
        case SIGSEGV:
            //TODO: kill all threads
            terminateProcess(target->mainThread, -1);
            return 0;

        default:
            return -EINVAL;
    }
}

//TODO: implement notify as soon as waitForEvents is implemented.
void Task::notify(ProcessControlBlock *p)
{    
}

