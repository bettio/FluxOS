/***************************************************************************
 *   Copyright 2015 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 09/11/2015                                                      *
 ***************************************************************************/

#include <arch/mips/core/userprocsmanager.h>
#include <arch/mips/core/cpuregistersframe.h>

#include <arch/mips/mm/pagingmanager.h>
#include <core/elfloader.h>
#include <task/archthreadsmanager.h>
#include <task/processcontrolblock.h>
#include <task/scheduler.h>
#include <task/task.h>
#include <task/threadcontrolblock.h>

//TODO: fix values here
#define USER_DEFAULT_STACK_ADDR 0x1000
#define MAX_ENV_VARS_NUMBER 10
#define MAX_ARGS_NUMBER 5
#define INIT_USER_STACK_SIZE 8192

//TODO: move away
#define userptr

int padToWord(int addr)
{
    return (addr & 0xFFFFFFFC) + 4;
}

void setupStackAndRegisters(RegistersFrame *frame, void *entryPoint, void *userSpaceStack, unsigned long userStackSize,
                            int argc, int argsSize, int envc, int envSize, int auxc, int auxSize,
                            char **argsList[], char **argsBlock,
                            char **envList[], char **envBlock,
                            char **auxList[], char **auxBlock)
{
    unsigned long varsBlockSize = sizeof(unsigned long) +
                                  sizeof(char *) * (argc + 1) + sizeof(char *) * (envc + 1) + sizeof(void *) * 2 * auxc
                                  + padToWord(argsSize) + padToWord(envSize) + padToWord(auxSize);
    void *sp =  (void *) (((unsigned long) userSpaceStack) + userStackSize - varsBlockSize);
    frame->registers[REGISTERS_FRAME_SP_REGISTER] = (unsigned long) sp;
    frame->pc = (unsigned long) entryPoint;

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

int calculateEnvironmentSize(userptr char *const env[], int *envVarsNum)
{
    int size = 0;
    for (int i = 0; i < MAX_ENV_VARS_NUMBER; i++) {
        //TODO: check env[i] address validity
        if (!env[i]) {
            *envVarsNum = i - 1;
            break;
        } else {
            //user
            size += strlen(env[i]) + 1;
        }
    }

    return size;
}

int calculateArgsListSize(userptr char *const args[], int *argsNum)
{
    int size = 0;
    for (int i = 0; i < MAX_ARGS_NUMBER; i++) {
        //TODO: check env[i] address validity
        if (!args[i]) {
            *argsNum = i - 1;
            break;
        } else {
            //user
            size += strlen(args[i]) + 1;
        }
    }

    return size;
}

void buildNewEnvironment(userptr const char *const env[], int envCount, userptr char *envTable[], userptr char *envBlock)
{
    for (int i = 0; i < envCount; i++) {
        strcpy(envBlock, env[i]);
        envTable[i] = envBlock;
        printk("envTable: %x\n", envTable[i]);
        envBlock += strlen(env[i]) + 1;
        printk("inserito: %s\n", envTable[i]);
    }
    envTable[envCount] = NULL;
}

void buildNewArgsList(userptr const char *const args[], int argsCount, userptr char *argsTable[], userptr char *argsBlock)
{
     for (int i = 0; i < argsCount; i++) {
        strcpy(argsBlock, args[i]);
        argsTable[i] = argsBlock;
        argsBlock += strlen(args[i]) + 1;
    }
    argsTable[argsCount] = NULL;
}

void buildAuxVector(userptr char *auxTable[], userptr char *auxBlock)
{
//TODO: implement this
}

void *createUserProcessStack(unsigned int size)
{
    return malloc(size);
}

int loadExecutable(const char *executablePath, void **entryPoint)
{
    ElfLoader loader;
    int res = loader.loadExecutableFile(executablePath);
    if (res < 0 || !loader.isValid()){
        printk("Cannot load executable file: %s error: %i\n", executablePath, res);
        return res;
    }
    *entryPoint = loader.entryPoint();
}

//TODO: remove this
void startRegsFrame(RegistersFrame *frame)
{
    asm("add $sp, $0, %1\n"
        "jr %0\n"
        : : "r" (frame->pc),
            "r" (frame->registers[REGISTERS_FRAME_SP_REGISTER])
       );
}

void setupInitProcessImage()
{
    RegistersFrame *regsFrame = new RegistersFrame;

    //TODO: no hardcoded here
    const char *initPath = "/sbin/init";
    const char *initCWD = "CWD=/";

    void *entryPoint;
    loadExecutable(initPath, &entryPoint);

    int argc = 1;
    int argsBlockSize = strlen(initPath) + 1;
    int envc = 1;
    int envBlockSize = strlen(initCWD) + 1;
    int auxc = 1; /* TODO: Implement here */
    int auxSize = 16;

    int userStackSize = padToWord(INIT_USER_STACK_SIZE + argsBlockSize + envBlockSize);

    void *userProcessStack = createUserProcessStack(userStackSize);
    char **argsList;
    char *argsBlock;
    char **envList;
    char *envBlock;
    char **auxList;
    char *auxBlock;

    setupStackAndRegisters(regsFrame, entryPoint, userProcessStack, userStackSize,
                           argc, argsBlockSize, envc, envBlockSize,
                           auxc, auxSize,
                           &argsList, &argsBlock, &envList, &envBlock,
                           &auxList, &auxBlock);

    const char *const args[2] = { initPath, NULL };
    buildNewArgsList(args, argc, argsList, argsBlock);
    const char *const env[2] = { initCWD, NULL };
    buildNewEnvironment(env, envc, envList, envBlock);
    buildAuxVector(auxList, auxBlock);

    Scheduler::init();
    ThreadControlBlock *thread = new ThreadControlBlock;
    Scheduler::threads->append(thread);
    ProcessControlBlock *process = Task::CreateNewTask("init");
    thread->parentProcess = process;
    thread->status = Running;

    //TODO: remove this
    startRegsFrame(regsFrame);
}

void UserProcsManager::createInitProcess()
{
    setupInitProcessImage();
}

//TODO: implement fork here
int UserProcsManager::fork(void *stack)
{
}

//TODO: not completely implemented
int execve(userptr const char *filename, userptr char *const argv[], userptr char *const envp[])
{
    RegistersFrame *regsFrame = new RegistersFrame;

    void *entryPoint;
    loadExecutable(filename, &entryPoint);

    int argc;
    int argsBlockSize = calculateArgsListSize(argv, &argc);
    int envc;
    int envBlockSize = calculateEnvironmentSize(envp, &envc);
    int auxc = 1; /* TODO: Implement here */
    int auxSize = 16;

    int userStackSize = padToWord(INIT_USER_STACK_SIZE + argsBlockSize + envBlockSize);

    void *userProcessStack = createUserProcessStack(userStackSize);
    char **argsList;
    char *argsBlock;
    char **envList;
    char *envBlock;
    char **auxList;
    char *auxBlock;

    setupStackAndRegisters(regsFrame, entryPoint, userProcessStack, userStackSize,
                           argc, argsBlockSize, envc, envBlockSize,
                           auxc, auxSize,
                           &argsList, &argsBlock, &envList, &envBlock,
                           &auxList, &auxBlock);

    buildNewArgsList(argv, argc, argsList, argsBlock);
    buildNewEnvironment(envp, envc, envList, envBlock);
    buildAuxVector(auxList, auxBlock);

    printk("panic here\n");
    while (1);

    //TODO: remove this
    startRegsFrame(regsFrame);
}
