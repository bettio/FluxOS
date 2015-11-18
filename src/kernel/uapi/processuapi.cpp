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
#include <task/scheduler.h>

#define pid_t unsigned long
#define uid_t unsigned long
#define gid_t unsigned long

void ProcessUAPI::init()
{
#ifndef ARCH_IA32
    SyscallsManager::registerSyscall(__NR_GETPID, (SYSCALL_FUNCTION_PTR) getpid);
    SyscallsManager::registerSyscall(__NR_GETPPID, (SYSCALL_FUNCTION_PTR) getppid);
    SyscallsManager::registerSyscall(__NR_GETSID, (SYSCALL_FUNCTION_PTR) getsid);
    SyscallsManager::registerSyscall(__NR_GETUID, (SYSCALL_FUNCTION_PTR) getuid);
    SyscallsManager::registerSyscall(__NR_GETEUID, (SYSCALL_FUNCTION_PTR) geteuid);
    SyscallsManager::registerSyscall(__NR_GETGID, (SYSCALL_FUNCTION_PTR) getgid);
    SyscallsManager::registerSyscall(__NR_GETEGID, (SYSCALL_FUNCTION_PTR) getegid);
    SyscallsManager::registerSyscall(__NR_SETSID, (SYSCALL_FUNCTION_PTR) setsid);
    SyscallsManager::registerSyscall(__NR_SETUID, (SYSCALL_FUNCTION_PTR) setuid);
    SyscallsManager::registerSyscall(__NR_SETRESUID, (SYSCALL_FUNCTION_PTR) setresuid);
    SyscallsManager::registerSyscall(__NR_SETGID, (SYSCALL_FUNCTION_PTR) setgid);
    SyscallsManager::registerSyscall(__NR_SETRESGID, (SYSCALL_FUNCTION_PTR) setresgid);
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

#undef pid_t
#undef uid_t
#undef gid_t
