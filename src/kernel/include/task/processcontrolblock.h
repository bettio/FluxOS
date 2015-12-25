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
 *   Name: processcontrolblock.h                                           *
 *   Date: 24/10/2007                                                      *
 ***************************************************************************/

#ifndef _TASK_PROCESSCONTROLBLOCK_H
#define _TASK_PROCESSCONTROLBLOCK_H
#include <KOOFCore>
#include <filesystem/vfs.h>
#include <filesystem/filedescriptor.h>
#include <kdef.h>

#include <ListWithHoles>

#define ROOT_UID 0
#define ROOT_GID 0

class MemoryContext;
class ThreadControlBlock;

enum TaskStatus{
    NOT_STARTED,
	READY,
	WAITING,
	SLEEPING,
	RUNNING,
	TERMINATED
};

class ProcessControlBlock
{
    public:
        unsigned int pid;
        unsigned int uid;
        unsigned int gid;
        TaskStatus status;
        int exitStatus;
        ProcessControlBlock *parent;
        ThreadControlBlock *mainThread;
        MemoryContext *memoryContext;

        void *dataSegmentStart;
        void *dataSegmentEnd;

        ListWithHoles <FileDescriptor *> *openFiles;
        VNode *currentWorkingDirNode;
        mode_t umask;

        inline mode_t calculateMode(mode_t mode)
        {
            return mode & (~umask);
        }
};

#endif

