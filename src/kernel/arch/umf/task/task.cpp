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
 *   Date: 27/11/2007                                                      *
 ***************************************************************************/

#include <task/task.h>
#include <arch/umf/core/hostsyscalls.h>
#include <filesystem/vnodemanager.h>
#include <cctype.h>
#include <core/printk.h>
#include <cstdlib.h>
#include <core/system.h>
#include <QList>

#include <lib/koof/intkeymap.h>

#include <syscall.h>

int cpid;
IntKeyMap<int> *Pids;
int currentTaskPid;

extern void SyscallsLoop(pid_t child, const char *name = 0);

using namespace FileSystem;

//TODO: string builder
void newTmpFileName(const char *name, char **tmpName)
{
    int nameLen = strlen(name);
    *tmpName = (char *) malloc(nameLen + sizeof(".tmp.") + 1);
    if (*tmpName){
        strcpy(*tmpName, ".tmp.");
        strcat(*tmpName, name);
    }
}

const char *fileName(const char *path)
{
    const char *name = path;
    
    while(*path){
        if (*path == '/') name = path + 1;
        path++;
    }
    
    return name;
}

int copyToHost(const char *path, const char *hostPath)
{
    char *buf = (char *) malloc(1024);
    if (buf == NULL){
        return -ENOMEM;
    }

    VNode *node;
    int fPos = 0;
    int hostFd = HostSysCalls::creat(hostPath, 0700);
    if (hostFd < 0){
        return hostFd;
    }
    int res = FileSystem::VFS::RelativePathToVnode(Task::CurrentTask()->CwdNode, path, &node);
    if (res < 0){
        HostSysCalls::close(hostFd);
        return res;
    }
    while(res = FS_CALL(node, read)(node, fPos, buf, 1024)){
        if (res < 0){
            VNodeManager::PutVnode(node);
            HostSysCalls::close(hostFd);
            return res;
        }
        HostSysCalls::write(hostFd, buf, res);
        fPos += res;
    }
    VNodeManager::PutVnode(node);
    HostSysCalls::close(hostFd);

    return 0;
}



void CreateNewProcess(char *tmpProcName, char *tmpProcParams)
{
    char *execName;
    newTmpFileName(fileName(tmpProcName), &execName);
    copyToHost(tmpProcName, execName);
    
    pid_t child = HostSysCalls::fork();
    
    if (child == 0){
        HostSysCalls::ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        int i = 0;
        bool prevIsSpace = true;
        QList<const char *> argsList;
        while(tmpProcParams[i]){
            if (isspace(tmpProcParams[i])){
                tmpProcParams[i] = 0;
                prevIsSpace = true;
            }else{
                if (prevIsSpace){
                   argsList.append(tmpProcParams + i);
                }
                prevIsSpace = false;
            }
            i++;
        }
        
        char **args = (char **) malloc((argsList.size() + 2) * sizeof(char *));
        args[0] = (char *) fileName(tmpProcName);
        for (i = 1; i <= argsList.size(); i++){
            args[i] = (char *) argsList.at(i - 1);
        }
        args[i] = NULL;

        //{(char *) fileName(tmpProcName), tmpProcParams, NULL};
        HostSysCalls::execve(execName, args, NULL);

    }else{
        Pids->Add(HostSysCalls::getpid(), Task::CreateNewTask(fileName(tmpProcName), true));

        SyscallsLoop(child, execName);
    }
}

void UMMStartInit()
{
    //Syscall init
    SysCall::Init();

    const char *tmpProcName = "/bin/init";
    char *tmpProcParams = "";
    
    char *execName;
    newTmpFileName(fileName(tmpProcName), &execName);
    copyToHost(tmpProcName, execName);
    
    pid_t child = HostSysCalls::fork();
    
    if (child == 0){
        HostSysCalls::ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        char *args[] = {(char *) fileName(tmpProcName), tmpProcParams, NULL};
        HostSysCalls::execve(execName, args, NULL);

    }else{
        Pids = new IntKeyMap<int>();
        Pids->Add(HostSysCalls::getpid(), Task::CreateNewTask(fileName(tmpProcName), true));

        SyscallsLoop(child, execName);
    }
}
