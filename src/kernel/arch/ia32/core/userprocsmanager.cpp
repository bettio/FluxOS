/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 06/12/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/core/userprocsmanager.h>

#include <arch/ia32/mm/pagingmanager.h>
#include <core/elfloader.h>
#include <task/archthreadsmanager.h>
#include <task/processcontrolblock.h>
#include <task/scheduler.h>
#include <task/task.h>
#include <task/threadcontrolblock.h>

char *UserProcsManager::executable;
char *UserProcsManager::args;

void UserProcsManager::processLoader()
{
    ElfLoader loader;
    int res = loader.loadExecutableFile(executable);
    if (res < 0 || !loader.isValid()){
        printk("Cannot load executable file: %s error: %i\n", executable, res);
        //exit
        Scheduler::currentThread()->status = UWaiting;
        Scheduler::currentThread()->parentProcess->status = TERMINATED;
	while(1);
    }

    register long tmpEax asm("%eax");

    asm(
        "pushl %1\n"
        "pushl %2\n"
        "pushl %3\n"
        "movl %%esp, %4\n"
        "pushl $0x23\n"
        "pushl %4\n"
        "pushf\n"
        "pushl $0x1B\n"
        "pushl %0\n"
        "movl $0x23, %4\n"
        "mov %4, %%ds\n"
        "mov %4, %%es\n"
        "mov %4, %%fs\n"
        "mov %4, %%gs\n"
        "iret\n" : : "r" (loader.entryPoint()), "r" (args), "r" (executable), "r" (1 + (strlen(args) != 0)), "r" (tmpEax));
    while(1);
}

void UserProcsManager::createInitProcess()
{
    executable = strdup("/bin/init");
    args = strdup("");
    ProcessControlBlock *process = Task::CreateNewTask("init");
    ThreadControlBlock *thread = ArchThreadsManager::createUserThread();
    ArchThreadsManager::makeExecutable(thread, UserProcsManager::processLoader, 0, 0);
    thread->parentProcess = process;
    thread->status = Running;

    #ifndef NO_MMU
        thread->addressSpaceTable = (void *) PagingManager::createPageDir(); 
    #endif

    Scheduler::threads->append(thread);
}

int UserProcsManager::createProcess(const char *path, const char *a, const char *envp)
{
    executable = strdup(path);
    args = strdup(a);
    ProcessControlBlock *process = Task::NewProcess((const char *)  a);
    ThreadControlBlock *thread = ArchThreadsManager::createUserThread();
    ArchThreadsManager::makeExecutable(thread, UserProcsManager::processLoader, 0, 0);
    thread->parentProcess = process;

    thread->addressSpaceTable = (void *) PagingManager::createPageDir();

    thread->status = Running;
    Scheduler::threads->append(thread);

    return process->pid;
}
