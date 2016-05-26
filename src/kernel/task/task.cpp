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
#include <uapi/fsuapi.h>
#include <arch.h>
#include <core/systemerrors.h>
#include <QMutexLocker>

#define ENABLE_DEBUG_MSG 0
#include <debugmacros.h>

QHash<int, ProcessControlBlock *> *Task::processes;
int Task::lastUsedPID;
QMutex Task::processesTableMutex;

bool Task::ProcessIterator::operator!=(const ProcessIterator other) const
{
    QMutexLocker locker(&Task::processesTableMutex);
    return other.hIt != hIt;
}

bool Task::ProcessIterator::operator==(const ProcessIterator other) const
{
    QMutexLocker locker(&Task::processesTableMutex);
    return other.hIt == hIt;
}

Task::ProcessIterator &Task::ProcessIterator::operator++()
{
    QMutexLocker locker(&Task::processesTableMutex);
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
    QMutexLocker locker(&processesTableMutex);
    return processes->value(pid) != NULL;
}

Task::ProcessIterator Task::processEnumerationBegin()
{
    QMutexLocker locker(&processesTableMutex);
    ProcessIterator it;
    it.hIt = processes->constBegin();
    return it;
}

Task::ProcessIterator Task::processEnumerationEnd()
{
    QMutexLocker locker(&processesTableMutex);
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
    process->euid = 0;
    process->gid = 0;
    process->egid = 0;
    process->pgid = 0;
    process->sid = 0;
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
    process->euid = parent->euid;
    process->gid = parent->gid;
    process->egid = parent->egid;
    process->pgid = parent->pgid;
    process->sid = parent->sid;
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

void Task::closeAllFiles(ProcessControlBlock *process, int conditionalFlag)
{
    for (int i = 0; i < process->openFiles->size(); i++){
        if ((process->openFiles->at(i) != NULL) &&  ((process->openFiles->at(i)->flags & conditionalFlag) == conditionalFlag)) {
            //TODO: do not use UAPI close here
            FSUAPI::close(i);
        }
    }
}

int Task::terminateProcess(ThreadControlBlock *thread, int exitStatus)
{
    if (thread->parentProcess && thread->parentProcess->pid == 1) {
        kernelPanic("Killed init.");
    }

    ProcessControlBlock *process = thread->parentProcess;
    delete process->memoryContext;
    closeAllFiles(process);
    FileSystem::VNodeManager::PutVnode(process->currentWorkingDirNode);
    process->exitStatus = exitStatus;
    process->status = TERMINATED;
    notify(process->parent);
    thread->status = UWaiting;
    
    return 0;
}

//TODO: implement notify as soon as waitForEvents is implemented.
void Task::notify(ProcessControlBlock *p)
{    
}

ProcessControlBlock *Task::process(int pid)
{
    QMutexLocker locker(&processesTableMutex);
    return processes->value(pid);
}


void Task::deleteProcess(int pid)
{
    QMutexLocker locker(&processesTableMutex);
    ProcessControlBlock *p = processes->value(pid);
    if (IS_NULL_PTR(p)) {
        printk("error: cannot find process %i\n", pid);
        return;
    }
    Task::processes->remove(pid);
    delete p->openFiles;
    delete p;
}
