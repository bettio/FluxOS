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

#include <task/userprocessimage.h>

#include <cstring.h>
#include <core/elfloader.h>
#include <core/printk.h>
#include <task/archthreadsmanager.h>
#include <task/processcontrolblock.h>
#include <task/scheduler.h>
#include <task/userprocsmanager.h>
#include <task/task.h>
#include <task/threadcontrolblock.h>

#define INIT_USER_STACK_SIZE 8192
#define MAX_ENV_VARS_NUMBER 10
#define MAX_ARGS_NUMBER 5

static inline int padToWord(int addr)
{
    return (addr & 0xFFFFFFFC) + 4;
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

int UserProcessImage::loadExecutable(const char *executablePath, void **entryPoint)
{
    ElfLoader loader;
    int res = loader.loadExecutableFile(executablePath);
    if (res < 0 || !loader.isValid()){
        printk("Cannot load executable file: %s error: %i\n", executablePath, res);
        return res;
    }
    *entryPoint = loader.entryPoint();
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

int UserProcessImage::setupInitProcessImage()
{
    RegistersFrame *regsFrame = UserProcsManager::createNewRegistersFrame();

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

    void *userProcessStack = UserProcsManager::createUserProcessStack(userStackSize);
    char **argsList;
    char *argsBlock;
    char **envList;
    char *envBlock;
    char **auxList;
    char *auxBlock;

    UserProcsManager::setupStackAndRegisters(regsFrame, entryPoint, userProcessStack, userStackSize,
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
    UserProcsManager::startRegsFrame(regsFrame);
}

//TODO: not completely implemented
int UserProcessImage::execve(userptr const char *filename, userptr char *const argv[], userptr char *const envp[])
{
    RegistersFrame *regsFrame = UserProcsManager::createNewRegistersFrame();

    void *entryPoint;
    loadExecutable(filename, &entryPoint);

    int argc;
    int argsBlockSize = calculateArgsListSize(argv, &argc);
    int envc;
    int envBlockSize = calculateEnvironmentSize(envp, &envc);
    int auxc = 1; /* TODO: Implement here */
    int auxSize = 16;

    int userStackSize = padToWord(INIT_USER_STACK_SIZE + argsBlockSize + envBlockSize);

    void *userProcessStack = UserProcsManager::createUserProcessStack(userStackSize);
    char **argsList;
    char *argsBlock;
    char **envList;
    char *envBlock;
    char **auxList;
    char *auxBlock;

    UserProcsManager::setupStackAndRegisters(regsFrame, entryPoint, userProcessStack, userStackSize,
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
    UserProcsManager::startRegsFrame(regsFrame);
}
