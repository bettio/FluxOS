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
 *   Name: task.h                                                          *
 ***************************************************************************/

#ifndef _TASK_TASK_H
#define _TASK_TASK_H

#define SIGKILL 9
#define SIGSEGV  11

#include <task/ProcessRef.h>
#include <task/processcontrolblock.h>

#include <QHash>
#include <QMutex>
#include <SpinLock>
#include <arch.h>

class ThreadControlBlock;

class Task{
	public:
            class ProcessIterator
            {
                public:
                    ~ProcessIterator();
                    ProcessControlBlock *processPtr;

                    bool operator!=(const ProcessIterator other) const;
                    bool operator==(const ProcessIterator other) const;
                    ProcessIterator &operator++();
                    inline int pid() { return processPtr->pid; }
                    inline ProcessControlBlock *process() { return processPtr; }
            };

        static void init();
        static ProcessIterator processEnumerationBegin();
        static ProcessIterator processEnumerationEnd();
        static ProcessControlBlock *CreateNewTask();
        static ProcessControlBlock *NewProcess();
        static bool isValidPID(int pid);
        static void closeAllFiles(ProcessControlBlock *process, int conditionalFlag = 0);
        static int terminateProcess(ThreadControlBlock *thread, int exitStatus);
        static void notify(ProcessControlBlock *p);

        static ProcessControlBlock *getProcess(int pid);
        static void putProcess(ProcessControlBlock *process);
        static ProcessControlBlock *referenceProcess(ProcessControlBlock *process);

        static ProcessRef getProcessRef(int pid);

        static void removePid(int pid);

        private:
            static int lastUsedPID;
            static QMutex processesTableMutex;
            static QMutex processesLinkedListMutex;
            static QHash<int, ProcessControlBlock *> *processes;
            static ProcessControlBlock *firstProcess;
            static ProcessControlBlock *lastProcess;

        static void deleteProcess(ProcessControlBlock *);
};

inline ProcessRef::ProcessRef(ProcessControlBlock *process)
{
    m_process = Task::referenceProcess(process);
}

inline ProcessRef::ProcessRef(const ProcessRef &procRef)
{
    m_process = Task::referenceProcess(procRef.m_process);
}

inline ProcessRef::~ProcessRef()
{
    Task::putProcess(m_process);
}

inline ProcessControlBlock *ProcessRef::operator->()
{
    return m_process;
}

inline bool ProcessRef::isValid() const
{
    return m_process != NULL;
}

#endif
