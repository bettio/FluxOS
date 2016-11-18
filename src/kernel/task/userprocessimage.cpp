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
#include <core/systemerrors.h>
#include <mm/memorycontext.h>
#include <mm/usermemoryops.h>
#include <task/archthreadsmanager.h>
#include <task/processcontrolblock.h>
#include <task/scheduler.h>
#include <task/auxdata.h>
#include <task/userprocsmanager.h>
#include <task/task.h>
#include <task/threadcontrolblock.h>

#include <arch.h>
#ifdef ARCH_IA32_NATIVE
#include <arch/ia32/mm/pagingmanager.h>
#endif
#ifdef ARCH_MIPS
 #include <arch/mips/mm/pagingmanager.h>
#endif

#define INIT_USER_STACK_SIZE 8192
#define MAX_ENV_VARS_NUMBER 10
#define MAX_ENV_VAR_LEN 256
#define MAX_ARGS_NUMBER 5
#define MAX_ARG_LEN 256
#define AUX_TABLE_SIZE 13

static inline int padToWord(int addr)
{
    return (addr & 0xFFFFFFFC) + 4;
}

int UserProcessImage::loadExecutable(const char *executablePath, AuxData *auxData, void **entryPoint)
{
    ElfLoader loader;
    int res = loader.loadExecutableFile(executablePath, auxData);
    if (UNLIKELY(res < 0)) {
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
        const char *srcString;
        int ret = getFromUser(&srcString, (const char **) &srcStringsVect[i]);
        if (UNLIKELY(ret < 0)) {
            return ret;
        }
        int len = strnlenUser(srcString, remainingSize - 1);
        if (UNLIKELY(len < 0)) {
            //bad string here, return with an error value
            return len;
        }
        int size = len + 1;
        ret = strncpyFromUser(destStringsBlock, srcStringsVect[i], size);
        if (UNLIKELY(ret < 0)) {
            //failed to copy string to dest
            return ret;
        }
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
        const char *srcString;
        int ret = getFromUser(&srcString, (const char **) &v[i]);
        if (UNLIKELY(ret < 0)) {
            return ret;
        }
        if (!srcString) {
            *num = i;
            break;
        } else {
            int len = strnlenUser(srcString, maxStrLen);
            if (UNLIKELY(len < 0)) {
                //bad string here for some reason, return an error
                return len;
            }
            size += len + 1;
        }
    }

    return size;
}

int UserProcessImage::auxVectorSizeAndCount(AuxData *auxdata, int *count, int *blockSize)
{
    int tmpSize = 0;

    tmpSize += strlen(auxdata->executableFileName) + 1;

    *count = AUX_TABLE_SIZE;
    *blockSize = tmpSize;

    return 0;
}

void UserProcessImage::buildAuxVector(AuxData *auxdata, ProcessControlBlock *process, userptr char *auxTable, userptr char *auxBlock)
{
    Elf32_auxv_t auxVec[14];

    auxVec[0].a_type = AT_HWCAP;
    auxVec[0].a_un.a_val = 0;

    auxVec[1].a_type = AT_PAGESZ;
    auxVec[1].a_un.a_val = PAGE_SIZE;

    auxVec[2].a_type = AT_PHDR;
    auxVec[2].a_un.a_val = auxdata->programHeaderAddress;

    auxVec[3].a_type = AT_PHENT;
    auxVec[3].a_un.a_val = auxdata->programHeaderEntrySize;

    auxVec[4].a_type = AT_PHNUM;
    auxVec[4].a_un.a_val = auxdata->programHeaderEntriesCount;

    auxVec[5].a_type = AT_BASE;
    auxVec[5].a_un.a_val = auxdata->interpreterBaseAddress;

    auxVec[6].a_type = AT_ENTRY;
    auxVec[6].a_un.a_val = auxdata->entryPointAddress;

    auxVec[7].a_type = AT_UID;
    auxVec[7].a_un.a_val = process->uid;

    auxVec[8].a_type = AT_EUID;
    auxVec[8].a_un.a_val  = process->euid;

    auxVec[9].a_type = AT_GID;
    auxVec[9].a_un.a_val = process->gid;

    auxVec[10].a_type = AT_EGID;
    auxVec[10].a_un.a_val = process->egid;

    auxVec[11].a_type = AT_EXECFN;
    auxVec[11].a_un.a_val = (uint32_t) auxBlock;
    memcpyToUser(auxBlock, auxdata->executableFileName, strlen(auxdata->executableFileName) + 1);
    free(auxdata->executableFileName);

    auxVec[12].a_type = AT_NULL;
    auxVec[12].a_un.a_val = 0;

    memcpyToUser(auxTable, auxVec, sizeof(Elf32_auxv_t) * AUX_TABLE_SIZE);
}

int UserProcessImage::setupInitProcessImage()
{
    ThreadControlBlock *thread = Scheduler::currentThread();
    ProcessControlBlock *process = Task::CreateNewTask();
    thread->parentProcess = process;

    RegistersFrame *regsFrame = UserProcsManager::createNewRegistersFrame();

    //TODO: no hardcoded here
    const char *initPath = strdup("/sbin/init");
    const char *initCWD = "CWD=/";
    AuxData *auxdata = new AuxData;
    if (IS_NULL_PTR(auxdata)) {
        return -ENOMEM;
    }
    auxdata->executableFileName = strdup(initPath);
    if (IS_NULL_PTR(auxdata->executableFileName)) {
        return -ENOMEM;
    }

    void *entryPoint;
    if (loadExecutable(initPath, auxdata, &entryPoint) < 0) {
        kernelPanic("cannot start init process");
    }

    int argc = 1;
    int argsBlockSize = strlen(initPath) + 1;
    int envc = 1;
    int envBlockSize = strlen(initCWD) + 1;
    int auxc;
    int auxBlockSize;
    thread->parentProcess->cmdline = strdup(initPath);
    thread->parentProcess->cmdlineSize = argsBlockSize;

    auxVectorSizeAndCount(auxdata, &auxc, &auxBlockSize);

    int userStackSize = padToWord(INIT_USER_STACK_SIZE + argsBlockSize + envBlockSize + auxBlockSize);

    void *userProcessStack = UserProcsManager::createUserProcessStack(userStackSize);
    char **argsList;
    char *argsBlock;
    char **envList;
    char *envBlock;
    char *auxList;
    char *auxBlock;

    UserProcsManager::setupStackAndRegisters(regsFrame, entryPoint, userProcessStack, userStackSize,
                           argc, argsBlockSize, envc, envBlockSize,
                           auxc, auxBlockSize,
                           &argsList, &argsBlock, &envList, &envBlock,
                           &auxList, &auxBlock);

    const char *const args[2] = { initPath, NULL };
    buildNewArgsList(args, argc, argsList, argsBlock);
    const char *const env[2] = { initCWD, NULL };
    buildNewEnvironment(env, envc, envList, envBlock);
    buildAuxVector(auxdata, thread->parentProcess, auxList, auxBlock);

    if (thread->parentProcess->memoryContext->allocateAnonymousMemory(&thread->parentProcess->dataSegmentStart, 4096*128,
            (MemoryDescriptor::Permissions) (MemoryDescriptor::ReadPermission | MemoryDescriptor::WritePermission),
            MemoryContext::FixedHint) < 0)
    {
        printk("Error: cannot allocate data segment for brk\n");
        return -ENOMEM;
    }
    thread->parentProcess->dataSegmentEnd = (void *) (((unsigned long ) thread->parentProcess->dataSegmentStart) + 4096);

    UserProcsManager::makeUserThread(thread);
    UserProcsManager::startRegsFrame(regsFrame);

    return 0;
}

//TODO: not completely implemented
int UserProcessImage::execve(userptr const char *filename, userptr char *const argv[], userptr char *const envp[])
{
    int ret = 0;

    UserString fileNameBuf(filename, 1024);
    if (UNLIKELY(!fileNameBuf.isValid())) {
        return fileNameBuf.errorCode();
    }
    const char *executablePath = fileNameBuf.constData();
    if (UNLIKELY(argv == NULL)) {
        printk("execve: invalid arguments\n");
        return -EINVAL;
    }
    if (UNLIKELY(envp == NULL)) {
        printk("execve: invalid environment\n");
        return -EINVAL;
    }

    ThreadControlBlock *thread = Scheduler::currentThread();

    RegistersFrame *regsFrame = UserProcsManager::createNewRegistersFrame();

    int argc;
    int argsBlockSize = stringsVectorSize(argv, &argc, MAX_ARGS_NUMBER, MAX_ARG_LEN);
    int envc;
    int envBlockSize = stringsVectorSize(envp, &envc, MAX_ENV_VARS_NUMBER, MAX_ENV_VAR_LEN);
    int auxc;
    int auxBlockSize;

    char *tmpArgsBlock = (char *) malloc(argsBlockSize);
    if (IS_NULL_PTR(tmpArgsBlock)) {
        return -ENOMEM;
    }
    ret = copyUserspaceStringsVectorToBlock(tmpArgsBlock, argv, argsBlockSize);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    char *tmpEnvBlock = (char *) malloc(envBlockSize);
    if (IS_NULL_PTR(tmpEnvBlock)) {
        return -ENOMEM;
    }
    ret = copyUserspaceStringsVectorToBlock(tmpEnvBlock, envp, envBlockSize);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    AuxData *auxdata = new AuxData;
    if (IS_NULL_PTR(auxdata)) {
        return -ENOMEM;
    }
    auxdata->executableFileName = strdup(fileNameBuf.constData());
    if (IS_NULL_PTR(auxdata->executableFileName)) {
        return -ENOMEM;
    }

    //We don't need previous address space anymore, but we still keep it so in case of failure we just restore it
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

    // switch to the new address space because we are going to write there
    #ifndef NO_MMU
        PagingManager::changeAddressSpace((volatile uint32_t *) thread->addressSpaceTable);
    #endif

    void *entryPoint;
    ret = loadExecutable(executablePath, auxdata, &entryPoint);
    if (UNLIKELY(ret < 0)) {
        thread->addressSpaceTable = previousAddressSpace;
        thread->parentProcess->memoryContext = previousMemoryContext;
        #ifndef NO_MMU
            PagingManager::changeAddressSpace((volatile uint32_t *) thread->addressSpaceTable);
        #endif
        return ret;
    }

    auxVectorSizeAndCount(auxdata, &auxc, &auxBlockSize);

    int userStackSize = padToWord(INIT_USER_STACK_SIZE + argsBlockSize + envBlockSize + auxBlockSize);

    void *userProcessStack = UserProcsManager::createUserProcessStack(userStackSize);
    char **argsList;
    char *argsBlock;
    char **envList;
    char *envBlock;
    char *auxList;
    char *auxBlock;

    UserProcsManager::setupStackAndRegisters(regsFrame, entryPoint, userProcessStack, userStackSize,
                           argc, argsBlockSize, envc, envBlockSize,
                           auxc, auxBlockSize,
                           &argsList, &argsBlock, &envList, &envBlock,
                           &auxList, &auxBlock);

    buildStringsPtrVector(argsList, argsBlock, tmpArgsBlock, argc);
    buildStringsPtrVector(envList, envBlock, tmpEnvBlock, envc);

    buildAuxVector(auxdata, thread->parentProcess, auxList, auxBlock);

    free(tmpEnvBlock);
    free(auxdata);

    if (UNLIKELY(thread->parentProcess->memoryContext->allocateAnonymousMemory(&thread->parentProcess->dataSegmentStart, 4096*128,
            (MemoryDescriptor::Permissions) (MemoryDescriptor::ReadPermission | MemoryDescriptor::WritePermission),
            MemoryContext::FixedHint) < 0))
    {
        printk("Error: cannot allocate data segment for brk\n");
        thread->addressSpaceTable = previousAddressSpace;
        thread->parentProcess->memoryContext = previousMemoryContext;
        #ifndef NO_MMU
            PagingManager::changeAddressSpace((volatile uint32_t *) thread->addressSpaceTable);
        #endif
        return -ENOMEM;
    }
    thread->parentProcess->dataSegmentEnd = (void *) (((unsigned long ) thread->parentProcess->dataSegmentStart) + 4096);

    //Here begins no return point, if something goes wrong it would be better to crash instead
    if (thread->parentProcess->cmdline) {
        free((void *) thread->parentProcess->cmdline);
    }
    thread->parentProcess->cmdline = tmpArgsBlock;
    thread->parentProcess->cmdlineSize = argsBlockSize;

    //TODO: change this
    free(previousAddressSpace);
    //TODO: we should remove previous memory context too

    //Close all O_CLOEXEC marked files
    Task::closeAllFiles(thread->parentProcess, O_CLOEXEC);

    fileNameBuf.releaseMemory();

    UserProcsManager::startRegsFrame(regsFrame);

    return 0;
}
