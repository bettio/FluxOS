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
#include <BuddyAllocator.h>
#include <mm/userspacememorymanager.h>
#include <stdint.h>

class VNode;

class MemoryResource
{
};

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
        MemoryResource *resource;
        Flags flags;
        Permissions permissions;
};

class MemoryMappedFileDescriptor : public MemoryDescriptor
{
    public:
        VNode *node;
        unsigned long offset;
};

class CopyOnWriteAnonMemory : public MemoryResource
{
    public:
        uint8_t *pageRefCounter;
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
        ~MemoryContext();
        MemoryDescriptor *findMemoryDescriptor(void *address) const;
        QList<MemoryDescriptor *> *findMemoryDescriptorsByRange(void *low, void *hi) const;
        int countDescriptorsByRange(void *low, void *hi) const;
        void handlePageFault(void *faultAddress, void *faultPC, UserspaceMemoryManager::MemoryOperation op, UserspaceMemoryManager::PageFaultFlags flags);
        int insertMemoryDescriptor(MemoryDescriptor *descriptor);
        void *findEmptyMemoryExtent(void *baseAddress, unsigned long length, MemoryContext::MemoryAllocationHints hints);
        int allocateAnonymousMemory(void *baseAddress, unsigned long length, MemoryDescriptor::Permissions permissions, MemoryContext::MemoryAllocationHints hints);
        int allocateAnonymousMemory(void **baseAddress, unsigned long length, MemoryDescriptor::Permissions permissions, MemoryContext::MemoryAllocationHints hints);

        /**
         * Resize the memory region pointed by descriptor
         * @param descriptor a valid memory descriptor pointer
         * @param increment memory descriptor length increment/decrement
         */
        long resizeExtent(MemoryDescriptor *descriptor, long increment);

        /**
         * Update memory protection for a memory region pointed by a descriptor
         * @param descriptor a valid memory descriptor pointer
         * @param permissions new permissions
         */
        int updatePermissions(MemoryDescriptor *descriptor, MemoryDescriptor::Permissions permissions);

        void *mapFileSegmentToMemory(VNode *node, void *virtualAddress, unsigned long length, unsigned long fileOffset, MemoryDescriptor::Permissions permissions);
        int releaseDescriptor(MemoryDescriptor *d);
        MemoryDescriptor *makeCOWAnonymousMemory(MemoryDescriptor *descriptor);

    private:
        void *allocVirtualMemory(unsigned long size);
        void allocVirtualMemory(void *address, unsigned long size);
        void freeVirtualMemory(void *address, unsigned long size);

        QList<MemoryDescriptor *> *m_descriptors;
        int pageSize;
        int pageSizeShift;
        unsigned long userVirtualMemLowAddress;
        unsigned long userVirtualMemSize;
        BuddyAllocator m_vmemAlloc;
};

#endif
