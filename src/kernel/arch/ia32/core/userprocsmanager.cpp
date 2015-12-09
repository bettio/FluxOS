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
#include <mm/memorycontext.h>
#include <task/archthreadsmanager.h>
#include <task/processcontrolblock.h>
#include <task/scheduler.h>
#include <task/task.h>
#include <task/threadcontrolblock.h>

char *UserProcsManager::executable;
char *UserProcsManager::args;
void *regs;

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

    Scheduler::currentThread()->parentProcess->memoryContext->allocateAnonymousMemory((void *) (USER_DEFAULT_STACK_ADDR - 1024), 2048,
                                                    (MemoryDescriptor::Permissions) (MemoryDescriptor::ReadPermission | MemoryDescriptor::WritePermission),
                                                    MemoryContext::FixedHint);



    memset((void *) (USER_DEFAULT_STACK_ADDR - 1024), 0, 2048);
    PagingManager::changeRegionFlags(USERSPACE_LOWER_ADDR, USERSPACE_LEN, 4, 0);

    register long tmpEax asm("%eax");

    asm("movl $0xD0000000, %%esp\n" /* 0xD0000000 = USER_DEFAULT_STACK_ADDR */ 
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
    executable = strdup("/sbin/init");
    args = strdup("");
    ProcessControlBlock *process = Task::CreateNewTask("init");
    ThreadControlBlock *thread = ArchThreadsManager::createUserThread();
    ArchThreadsManager::makeExecutable(thread, UserProcsManager::processLoader, 0, 0);
    thread->parentProcess = process;
    thread->status = Running;

    #ifndef NO_MMU
        thread->addressSpaceTable = (void *) PagingManager::createPageDir(); 
    #endif

    process->mainThread = thread;
    Scheduler::threads->append(thread);
}

void UserProcsManager::setupChild()
{
    asm("movl %0, %%esp\n"
        "addl $32, %0\n" 
        "movl %0, 12(%%esp)\n"
        "popa\n"
        "movl $0, %%eax\n"
        "iret\n" : : "r" (regs));

}

int UserProcsManager::fork(void *stack)
{
    posix_memalign(&regs, 4096, 32+20 + 500);
    memcpy(regs, stack, 32+20);
    regs = (void *) (((char *) regs));

    ProcessControlBlock *process = Task::NewProcess("ufork");
    ThreadControlBlock *thread = ArchThreadsManager::createUserThread();
    ArchThreadsManager::makeExecutable(thread, setupChild, 0, 0);
    thread->parentProcess = process;
    thread->status = Running;

    PagingManager::changeRegionFlags(USERSPACE_LOWER_ADDR, USERSPACE_LEN, 0, 2, 1);
    thread->addressSpaceTable = (void *) PagingManager::clonePageDir(); 

    process->mainThread = thread;
    Scheduler::threads->append(thread);

    return process->pid;
}

int UserProcsManager::execve(const char *_filename, char *const _argv[], char *const _envp[])
{
    if (!_filename) {
        printk("Error: execve: null filename\n");
        return -EFAULT;
    }
    if (!_argv[1]) {
        printk("Error: execve: null argument\n");
        return -EFAULT;
    }
    const char *filename = strdup(_filename);
    const char *argv = strdup(_argv[1]);

    PagingManager::cleanUserspace();

    ElfLoader loader;
    int res = loader.loadExecutableFile(filename);
    if (res < 0 || !loader.isValid()){
        printk("Cannot load executable file: %s error: %i\n", filename, res);
        //exit
        Scheduler::currentThread()->status = UWaiting;
        Scheduler::currentThread()->parentProcess->status = TERMINATED;
        while(1);
    }

    Scheduler::currentThread()->parentProcess->memoryContext->allocateAnonymousMemory((void *) USER_DEFAULT_ARGS_ADDR, 4096,
                                                    (MemoryDescriptor::Permissions) (MemoryDescriptor::ReadPermission | MemoryDescriptor::WritePermission),
                                                    MemoryContext::FixedHint);

    strncpy((char *) USER_DEFAULT_ARGS_ADDR, argv, 4096);
    char *arg = (char *) USER_DEFAULT_ARGS_ADDR;

    Scheduler::currentThread()->parentProcess->memoryContext->allocateAnonymousMemory((void *) (USER_DEFAULT_STACK_ADDR - 1024), 2048,
                                                    (MemoryDescriptor::Permissions) (MemoryDescriptor::ReadPermission | MemoryDescriptor::WritePermission),
                                                    MemoryContext::FixedHint);


    memset((void *) (USER_DEFAULT_STACK_ADDR - 1024), 0, 2048);
    PagingManager::changeRegionFlags(USERSPACE_LOWER_ADDR, USERSPACE_LEN, 4, 0, 1);

    register long tmpEax asm("%eax");

    asm("movl $0xD0000000, %%esp\n" /* 0xD0000000 = USER_DEFAULT_STACK_ADDR */ 
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
        "iret\n" : : "r" (loader.entryPoint()), "r" (arg), "r" (filename), "r" (1 + (strlen(arg) != 0)), "r" (tmpEax));

    return 0;
}
