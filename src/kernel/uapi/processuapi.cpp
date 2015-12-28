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
 *   Name: processuapi.cpp                                                 *
 *   Date: 17/11/2015                                                      *
 ***************************************************************************/

#include <uapi/processuapi.h>
#include <arch.h>

#ifndef ARCH_IA32
#include <uapi/syscallsnr.h>
#endif
#include <core/syscallsmanager.h>
#include <mm/usermemoryops.h>
#include <task/scheduler.h>
#include <task/task.h>

#define pid_t unsigned long
#define uid_t unsigned long
#define gid_t unsigned long

void ProcessUAPI::init()
{
#ifndef ARCH_IA32
    SyscallsManager::registerSyscall(__NR_GETPID,(void *) getpid);
    SyscallsManager::registerSyscall(__NR_GETPPID,(void *) getppid);
    SyscallsManager::registerSyscall(__NR_GETSID,(void *) getsid);
    SyscallsManager::registerSyscall(__NR_GETUID,(void *) getuid);
    SyscallsManager::registerSyscall(__NR_GETEUID,(void *) geteuid);
    SyscallsManager::registerSyscall(__NR_GETGID,(void *) getgid);
    SyscallsManager::registerSyscall(__NR_GETEGID,(void *) getegid);
    SyscallsManager::registerSyscall(__NR_SETSID,(void *) setsid);
    SyscallsManager::registerSyscall(__NR_SETUID,(void *) setuid);
    SyscallsManager::registerSyscall(__NR_SETRESUID,(void *) setresuid);
    SyscallsManager::registerSyscall(__NR_SETGID,(void *) setgid);
    SyscallsManager::registerSyscall(__NR_SETRESGID,(void *) setresgid);
#endif
}

pid_t ProcessUAPI::getpid()
{
    return Scheduler::currentThread()->parentProcess->pid;
}

pid_t ProcessUAPI::getppid()
{
    if (Scheduler::currentThread()->parentProcess->parent) {
        return Scheduler::currentThread()->parentProcess->parent->pid;

    } else {
        return Scheduler::currentThread()->parentProcess->pid;
    }
}

//TODO: implement this
pid_t ProcessUAPI::getsid(pid_t pid)
{
    return 0;
}

uid_t ProcessUAPI::getuid()
{
    return Scheduler::currentThread()->parentProcess->uid;
}

//TODO: implement this
uid_t ProcessUAPI::geteuid()
{
    return 0;
}

gid_t ProcessUAPI::getgid()
{
    return Scheduler::currentThread()->parentProcess->gid;
}

//TODO: implement this
gid_t ProcessUAPI::getegid()
{
    return 0;
}

//TODO: implement this
pid_t ProcessUAPI::setsid()
{
    return 0;
}

int ProcessUAPI::setuid(uid_t uid)
{
    ProcessControlBlock *task = Scheduler::currentThread()->parentProcess;

    //TODO: use capability check
    if (task->uid == 0) {
        task->uid = uid;
        return 0;
    }else{
        return -EPERM;
    }
}

//TODO: implement this
int ProcessUAPI::setresuid(uid_t ruid, uid_t euid, uid_t suid)
{
    return -EFAULT;
}

int ProcessUAPI::setgid(gid_t gid)
{
    ProcessControlBlock *task = Scheduler::currentThread()->parentProcess;

    //TODO: use capability check
    if (task->uid == 0){
        task->gid = gid;
        return 0;
    }else{
        return -EPERM;
    }
}

//TODO: implement this
int ProcessUAPI::setresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
    return -EFAULT;
}

int ProcessUAPI::waitpid(pid_t pid, int *status, int options)
{
    if (UNLIKELY(!canWriteUserMemory(status, sizeof(int *)))) {
        return -EFAULT;
    }

    ProcessControlBlock *p;

    if (pid != (pid_t) -1) {
        p = Task::processes->value(pid);
        if (p == NULL || p->parent != Scheduler::currentThread()->parentProcess) {
            return -ECHILD;
        }

        while (p->status != TERMINATED) Scheduler::waitForEvents();

    } else {
        while (true) {
            for (Task::ProcessIterator it = Task::processEnumerationBegin(); it != Task::processEnumerationEnd(); ++it) {
                p = it.process();
                if ((p->status == TERMINATED) && (p->parent == Scheduler::currentThread()->parentProcess)) {
                    break;
                }
            }
            if ((p->status == TERMINATED) && (p->parent == Scheduler::currentThread()->parentProcess)){
                pid = p->pid;
                break;
            }
            Scheduler::waitForEvents();
        }
    }

    int ret = putToUser(p->exitStatus, status);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    Task::processes->remove(pid);
    delete p->openFiles;
    delete p;

    return pid;
}

#undef pid_t
#undef uid_t
#undef gid_t
