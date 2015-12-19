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

#include <task/userprocsmanager.h>

#include <arch/ia32/core/registersframe.h>
#include <arch/ia32/mm/pagingmanager.h>
#include <core/elfloader.h>
#include <mm/memorycontext.h>
#include <task/archthreadsmanager.h>
#include <task/processcontrolblock.h>
#include <task/scheduler.h>
#include <task/task.h>
#include <task/threadcontrolblock.h>

char *executable;
char *args;
void *regs;

static inline int padToWord(int addr)
{
    return (addr & 0xFFFFFFFC) + 4;
}

void processLoader()
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

void createInitProcess()
{
    executable = strdup("/sbin/init");
    args = strdup("");
    ProcessControlBlock *process = Task::CreateNewTask("init");
    ThreadControlBlock *thread = ArchThreadsManager::createUserThread();
    ArchThreadsManager::makeExecutable(thread, processLoader, 0, 0);
    thread->parentProcess = process;
    thread->status = Running;

    #ifndef NO_MMU
        thread->addressSpaceTable = (void *) PagingManager::createPageDir(); 
    #endif

    process->mainThread = thread;
    Scheduler::threads->append(thread);
}

void setupChild()
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

void UserProcsManager::setupStackAndRegisters(RegistersFrame *frame, void *entryPoint, void *userSpaceStack, unsigned long userStackSize,
                            int argc, int argsSize, int envc, int envSize, int auxc, int auxSize,
                            char **argsList[], char **argsBlock,
                            char **envList[], char **envBlock,
                            char **auxList[], char **auxBlock)
{
    unsigned long varsBlockSize = sizeof(unsigned long) +
                                  sizeof(char *) * (argc + 1) + sizeof(char *) * (envc + 1) + sizeof(void *) * 2 * auxc
                                  + padToWord(argsSize) + padToWord(envSize) + padToWord(auxSize);

    void *sp =  (void *) (((unsigned long) userSpaceStack) + userStackSize - varsBlockSize);
    frame->null = (uint32_t) sp;
    frame->eip = (uint32_t) entryPoint;

    unsigned long *vars = (unsigned long *) sp;
    /*
     MIPS ABI, user process stack:
     - Lower address
     - argc <- SP
     - pointers array to args
     - 0
     - pointers array to env vars
     - 0
     - aux vector
     - 0
     - Args block (args array point to this)
     - env block (env vars array point to this)
     - Higher address
    */
    int pos = 0;

    vars[pos] = argc;
    pos++;

    *argsList = (char **) &vars[pos];
    pos += argc + 1;

    *envList = (char **) &vars[pos];
    pos += envc + 1;

    *auxList = (char **) &vars[pos];
    pos += (auxc + 1)*2;

    *argsBlock = (char *) &vars[pos];
    pos += (argsSize / 4) + 1;

    *envBlock = (char *) &vars[pos];
    pos += (envSize / 4) + 1;

    *auxBlock = (char *) &vars[pos];
}

RegistersFrame *UserProcsManager::createNewRegistersFrame()
{
    return new RegistersFrame;
}

void *UserProcsManager::createUserProcessStack(unsigned int size)
{
    void *stackAddr = NULL;
    Scheduler::currentThread()->parentProcess->memoryContext->allocateAnonymousMemory(&stackAddr, size,
                                                    (MemoryDescriptor::Permissions) (MemoryDescriptor::ReadPermission | MemoryDescriptor::WritePermission),
                                                    MemoryContext::NoHints);
    return stackAddr;
}


void UserProcsManager::startRegsFrame(RegistersFrame *frame)
{
    register long tmpEax asm("%eax");

    asm(
        "pushl $0x23\n"
        "pushl %2\n"
        "pushf\n"
        "pushl $0x1B\n"
        "pushl %0\n"
        "movl $0x23, %1\n"
        "mov %1, %%ds\n"
        "mov %1, %%es\n"
        "mov %1, %%fs\n"
        "mov %1, %%gs\n"
        "iret\n" : : "r" (frame->eip), "r" (tmpEax), "r" (frame->null));
}
