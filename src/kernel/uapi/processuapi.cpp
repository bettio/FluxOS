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
#include <core/systemtimer.h>
#include <mm/usermemoryops.h>
#include <task/scheduler.h>
#include <task/task.h>
#include <task/threadsmanager.h>
#include <task/userprocsmanager.h>
#ifdef ARCH_IA32_NATIVE
#include <arch/ia32/mm/pagingmanager.h>
#endif
#ifdef ARCH_MIPS
 #include <arch/mips/mm/pagingmanager.h>
#endif

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

    SyscallsManager::registerSyscall(__NR_NANOSLEEP,(void *) nanosleep);
    SyscallsManager::registerSyscall(__NR_WAITPID,(void *) waitpid);
    SyscallsManager::registerSyscall(__NR_KILL,(void *) kill);
    SyscallsManager::registerSyscall(__NR_EXIT,(void *) exit);
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

pid_t ProcessUAPI::getsid(pid_t pid)
{
    return Scheduler::currentThread()->parentProcess->sid;
}

uid_t ProcessUAPI::getuid()
{
    return Scheduler::currentThread()->parentProcess->uid;
}

uid_t ProcessUAPI::geteuid()
{
    return Scheduler::currentThread()->parentProcess->euid;
}

gid_t ProcessUAPI::getgid()
{
    return Scheduler::currentThread()->parentProcess->gid;
}

gid_t ProcessUAPI::getegid()
{
    return Scheduler::currentThread()->parentProcess->egid;
}

pid_t ProcessUAPI::setsid()
{
    Scheduler::currentThread()->parentProcess->sid = Scheduler::currentThread()->parentProcess->pid;
    return Scheduler::currentThread()->parentProcess->sid;
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

int ProcessUAPI::fork(void *stack)
{
    ThreadControlBlock *thread = NULL;
    int ret = UserProcsManager::forkThread(stack, &thread);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    ProcessControlBlock *process = Task::NewProcess();
    if (IS_NULL_PTR(process)) {
        delete thread;
        return -ENOMEM;
    }
    thread->parentProcess = Task::referenceProcess(process);
    process->mainThread = thread;

#ifdef ARCH_IA32
    PagingManager::changeRegionFlags(USERSPACE_LOWER_ADDR, USERSPACE_LEN, 0, 2, 1);
    thread->addressSpaceTable = (void *) PagingManager::clonePageDir(); 
#endif

    Scheduler::inhibitPreemption();
    Scheduler::threads->append(thread);
    Scheduler::restorePreemption();

    return process->pid;
}

int ProcessUAPI::nanosleep(const struct timespec *req, struct timespec *rem)
{
    struct timespec requestedInterval;
    struct timespec remainingInterval;
    int memcpyRet = memcpyFromUser(&requestedInterval, req, sizeof(struct timespec));
    if (UNLIKELY(memcpyRet < 0)) {
        return memcpyRet;
    }

    int ret = SystemTimer::sleep(&requestedInterval, &remainingInterval);

    memcpyRet = memcpyFromUser(&remainingInterval, rem, sizeof(struct timespec));
    if (UNLIKELY(memcpyRet < 0)) {
        return memcpyRet;
    }

    return ret;
}

int ProcessUAPI::waitpid(pid_t pid, int *status, int options)
{
    if (UNLIKELY(!canWriteUserMemory(status, sizeof(int *)))) {
        return -EFAULT;
    }

    int exitStatus;

    if (pid != (pid_t) -1) {
        ProcessRef p = Task::getProcessRef(pid);
        if (!p.isValid() || p->parent != Scheduler::currentThread()->parentProcess) {
            return -ECHILD;
        }

        while (p->status != TERMINATED) Scheduler::waitForEvents();
        exitStatus = p->exitStatus;

    } else {
        ProcessControlBlock *p;

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

        exitStatus = p->exitStatus;
    }

    int ret = putToUser(exitStatus, status);
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    Task::removePid(pid);

    return pid;
}

int ProcessUAPI::kill(pid_t pid, int signal)
{
    ProcessRef target = Task::getProcessRef(pid);
    if (!target.isValid()) {
        return -ESRCH;
    }

    ProcessControlBlock *currentProcess = Scheduler::currentThread()->parentProcess;
    if (currentProcess->uid != ROOT_UID){
        if (currentProcess->uid != target->uid) return -EPERM;
    }

    switch (signal){
        case SIGKILL:
            //TODO: kill all threads
            Task::terminateProcess(target->mainThread, -1);
            return 0;
        case SIGSEGV:
            //TODO: kill all threads
            Task::terminateProcess(target->mainThread, -1);
            return 0;

        default:
            return -EINVAL;
    }
}

void ProcessUAPI::exit(int exitStatus)
{
    ThreadsManager::terminateThread(Scheduler::currentThread(), exitStatus);
    while(1);
}

#undef pid_t
#undef uid_t
#undef gid_t
