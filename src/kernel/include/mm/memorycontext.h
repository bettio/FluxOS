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
 *   Name: memorycontext.h                                                 *
 *   Date: 20/11/2015                                                      *
 ***************************************************************************/

#ifndef _MM_MEMORY_CONTEXT_H_
#define _MM_MEMORY_CONTEXT_H_

#include <QList>
#include <mm/userspacememorymanager.h>

class VNode;

class MemoryDescriptor
{
    public:
        enum Permissions
        {
            NoAccess = 0,
            ReadPermission = 1,
            WritePermission = 2,
            ExecutePermission = 4
        };

        enum Flags
        {
            AnonymousMemory = 1,
            MemoryMappedFile = 2
        };

        void *baseAddress;
        unsigned long length;
        Flags flags;
        Permissions permissions;
};

class MemoryMappedFileDescriptor : public MemoryDescriptor
{
    public:
        VNode *node;
        unsigned long offset;
};

class MemoryContext
{
    public:
        enum MemoryAllocationHints
        {
            NoHints = 0,
            FixedHint = 1
        };

        MemoryContext();
        void handlePageFault(void *faultAddress, void *faultPC, UserspaceMemoryManager::MemoryOperation op, UserspaceMemoryManager::PageFaultFlags flags);
        int insertMemoryDescriptor(MemoryDescriptor *descriptor);
        void *findMemoryExtent(void *baseAddress, unsigned long length, MemoryContext::MemoryAllocationHints hints);
        int allocateAnonymousMemory(void *baseAddress, unsigned long length, MemoryDescriptor::Permissions permissions, MemoryContext::MemoryAllocationHints hints);

    private:
        QList<MemoryDescriptor *> *m_descriptors;
};

#endif
