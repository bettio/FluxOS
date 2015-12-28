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

#include <task/processcontrolblock.h>
#include <QHash>
#include <arch.h>

class ThreadControlBlock;

class Task{
	public:
            class ProcessIterator
            {
                public:
                    QHash<int, ProcessControlBlock *>::const_iterator hIt;

                    bool operator!=(const ProcessIterator other) const;
                    bool operator==(const ProcessIterator other) const;
                    ProcessIterator &operator++();
                    inline int pid() { return hIt.value()->pid; }
                    inline ProcessControlBlock *process() { return hIt.value(); }
            };

        static void init();
        static ProcessIterator processEnumerationBegin();
        static ProcessIterator processEnumerationEnd();
        static ProcessControlBlock *CreateNewTask();
        static ProcessControlBlock *NewProcess();
        static QHash<int, ProcessControlBlock *> *processes;
        static bool isValidPID(int pid);
        static void closeAllFiles(ProcessControlBlock *process);
        static void exit(int exitStatus);
        static int terminateProcess(ThreadControlBlock *thread, int exitStatus);
        static int kill(int pid, int signal);
        static void notify(ProcessControlBlock *p);
        private:
            static int lastUsedPID;
};

#endif
