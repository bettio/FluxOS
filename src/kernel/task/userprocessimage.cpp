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
#include <arch/ia32/mm/pagingmanager.h>
#include <core/elfloader.h>
#include <core/printk.h>
#include <core/systemerrors.h>
#include <mm/memorycontext.h>
#include <task/archthreadsmanager.h>
#include <task/processcontrolblock.h>
#include <task/scheduler.h>
#include <task/userprocsmanager.h>
#include <task/task.h>
#include <task/threadcontrolblock.h>

#define INIT_USER_STACK_SIZE 8192
#define MAX_ENV_VARS_NUMBER 10
#define MAX_ENV_VAR_LEN 256
#define MAX_ARGS_NUMBER 5
#define MAX_ARG_LEN 256

static inline int padToWord(int addr)
{
    return (addr & 0xFFFFFFFC) + 4;
}

int UserProcessImage::loadExecutable(const char *executablePath, void **entryPoint)
{
    ElfLoader loader;
    int res = loader.loadExecutableFile(executablePath);
    if (res < 0) {
        printk("Cannot load executable file: %s error: %i\n", executablePath, res);
        return res;
    }
    *entryPoint = loader.entryPoint();
    return 0;
}

void UserProcessImage::buildNewEnvironment(userptr const char *const env[], int envCount, userptr char *envTable[], userptr char *envBlock)
{
    for (int i = 0; i < envCount; i++) {
        strcpy(envBlock, env[i]);
        envTable[i] = envBlock;
        envBlock += strlen(env[i]) + 1;
    }
    envTable[envCount] = NULL;
}

void UserProcessImage::buildNewArgsList(userptr const char *const args[], int argsCount, userptr char *argsTable[], userptr char *argsBlock)
{
     for (int i = 0; i < argsCount; i++) {
        strcpy(argsBlock, args[i]);
        argsTable[i] = argsBlock;
        argsBlock += strlen(args[i]) + 1;
    }
    argsTable[argsCount] = NULL;
}

int UserProcessImage::copyUserspaceStringsVectorToBlock(char *destStringsBlock, userptr const char *const srcStringsVect[], int destStringsBlockSize)
{
    int i = 0;
    int remainingSize = destStringsBlockSize;

    while (srcStringsVect[i]) {
        int size = strnlen(srcStringsVect[i], remainingSize - 1) + 1;
        strncpy(destStringsBlock, srcStringsVect[i], size);
        destStringsBlock += size;
        i++;
    }

    return 0;
}

void UserProcessImage::buildStringsPtrVector(userptr char *destArgsTable[], userptr char *destArgsBlock, const char *srcArgsBlock, int argsCount)
{
    for (int i = 0; i < argsCount; i++) {
        destArgsTable[i] = destArgsBlock;
        int size = strlen(srcArgsBlock) + 1;
        strcpy(destArgsBlock, srcArgsBlock);
        destArgsBlock += size;
        srcArgsBlock += size;
    }
    destArgsTable[argsCount] = NULL;
}

int UserProcessImage::stringsVectorSize(userptr char *const v[], int *num, int maxNum, int maxStrLen)
{
    int size = 0;
    for (int i = 0; i < maxNum; i++) {
        //TODO: check env[i] address validity
        if (!v[i]) {
            *num = i;
            break;
        } else {
            //TODO: user
            size += strnlen(v[i], maxStrLen) + 1;
        }
    }

    return size;
}


void UserProcessImage::buildAuxVector(userptr char *auxTable[], userptr char *auxBlock)
{
//TODO: implement this
}

int UserProcessImage::setupInitProcessImage()
{
    Scheduler::init();
    ThreadControlBlock *thread = new ThreadControlBlock;
    Scheduler::threads->append(thread);
    ProcessControlBlock *process = Task::CreateNewTask("init");
    thread->parentProcess = process;
    thread->status = Running;

    RegistersFrame *regsFrame = UserProcsManager::createNewRegistersFrame();

    //TODO: no hardcoded here
    const char *initPath = "/sbin/init";
    const char *initCWD = "CWD=/";

    void *entryPoint;
    if (loadExecutable(initPath, &entryPoint) < 0) {
        kernelPanic("cannot start init process");
    }

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

    //TODO: remove this
    UserProcsManager::startRegsFrame(regsFrame);

    return 0;
}

//TODO: not completely implemented
int UserProcessImage::execve(userptr const char *filename, userptr char *const argv[], userptr char *const envp[])
{
    int ret = 0;

    if (filename == NULL) {
        printk("Invalid executable path\n");
        return -ENOEXEC;
    }
    if (argv == NULL) {
        printk("Invalid arguments\n");
        return -EINVAL;
    }
    if (envp == NULL) {
        printk("Invalid environment\n");
        return -EINVAL;
    }
    const char *executablePath = strdup(filename);

    RegistersFrame *regsFrame = UserProcsManager::createNewRegistersFrame();

    int argc;
    int argsBlockSize = stringsVectorSize(argv, &argc, MAX_ARGS_NUMBER, MAX_ARG_LEN);
    int envc;
    int envBlockSize = stringsVectorSize(envp, &envc, MAX_ENV_VARS_NUMBER, MAX_ENV_VAR_LEN);
    int auxc = 1; /* TODO: Implement here */
    int auxSize = 16;

    char *tmpArgsBlock = (char *) malloc(argsBlockSize);
    ret = copyUserspaceStringsVectorToBlock(tmpArgsBlock, argv, argsBlockSize);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    char *tmpEnvBlock = (char *) malloc(envBlockSize);
    ret = copyUserspaceStringsVectorToBlock(tmpEnvBlock, envp, envBlockSize);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    //We don't need previous address space anymore, but we still keep it so in case of failure we just restore it
    ThreadControlBlock *thread = Scheduler::currentThread();
    MemoryContext *previousMemoryContext = thread->parentProcess->memoryContext;
    void *previousAddressSpace = thread->addressSpaceTable;
    void *newAddressSpace = (void *) PagingManager::createPageDir();
    if (IS_NULL_PTR(newAddressSpace)) {
        printk("Error: cannot create a new address space");
        return -ENOMEM;
    }
    MemoryContext *newMemoryContext = new MemoryContext();
    if (IS_NULL_PTR(newMemoryContext)) {
        //TODO newAddressSpace
        printk("Error: cannot create a new memory context\n");
        return -ENOMEM;
    }
    thread->addressSpaceTable = newAddressSpace;
    thread->parentProcess->memoryContext = newMemoryContext;

    void *entryPoint;
    ret = loadExecutable(executablePath, &entryPoint);
    if (UNLIKELY(ret < 0)) {
        thread->addressSpaceTable = previousAddressSpace;
        thread->parentProcess->memoryContext = previousMemoryContext;
        return ret;
    }

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

    buildStringsPtrVector(argsList, argsBlock, tmpArgsBlock, argc);
    buildStringsPtrVector(envList, envBlock, tmpEnvBlock, envc);

    buildAuxVector(auxList, auxBlock);

    if (thread->parentProcess->memoryContext->allocateAnonymousMemory(&thread->parentProcess->dataSegmentStart, 4096,
            (MemoryDescriptor::Permissions) (MemoryDescriptor::ReadPermission | MemoryDescriptor::WritePermission),
            MemoryContext::FixedHint) < 0)
    {
        printk("Error: cannot allocate data segment for brk\n");
        return -ENOMEM;
    }
    thread->parentProcess->dataSegmentEnd = (void *) (((unsigned long ) thread->parentProcess->dataSegmentStart) + 4096);

    //TODO: remove this
    UserProcsManager::startRegsFrame(regsFrame);

    return 0;
}
