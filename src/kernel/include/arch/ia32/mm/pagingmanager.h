/***************************************************************************
 *   Copyright 2005 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: pagingmanager.h                                                 *
 *   Date: 23/09/2005                                                      *
 ***************************************************************************/

#ifndef _IA32_MM_PAGINGMANAGER_H_
#define _IA32_MM_PAGINGMANAGER_H_

#include <stdint.h>

#define MISSING_PAGE 0
#define KERNEL_STD_PAGE PagingManager::Present | PagingManager::Write

#define PAGEDIR_ENTRIES 1024
#define PAGETABLE_ENTRIES 1024
#define PAGE_BOUNDARY 4096
#define PAGE_SIZE 4096

class PagingManager
{
    public:
        enum PageFlags
        {
            Present = 1,
            Write = 2,
            User = 4,
            WriteThrough = 8,
            PageLevelCacheDisable = 16,
            Accessed = 32,
            Dirty = 64,
            PageSize = 128,
            PAT = 128,
            Global = 256
        };

        static void init();
        inline static void setCR3(uint32_t cr3reg) { asm volatile("movl %0, %%cr3\n" : : "r"(cr3reg)); }
        static void mapPhysicalMemoryRegion(volatile uint32_t *pageDir, uint32_t physAddr, uint32_t virtualAddr, uint32_t len);
        static volatile uint32_t *createEmptyPageDir();
        static volatile uint32_t *clonePageDir();
        static volatile uint32_t *createEmptyPageTable();
        static volatile uint32_t *createPageDir();
        static void newPage(uint32_t addr, unsigned long flags = 0);
        static void changeAddressSpace(volatile uint32_t *pageDir, bool forceUpdate = false);
        static void cloneKernelSpace(volatile uint32_t *pageDir);
        static void *mapPhysicalMemory(uint32_t physAddr, int len);
        inline static unsigned int addrToPageDirIndex(uint32_t addr){ return addr >> 22; }
        inline static unsigned int addrToPageTableIndex(uint32_t addr){ return (addr >> 12) & 0x3FF; }
        inline static uint32_t physicalAddressOf(void *ptr) { return ((volatile uint32_t *) ((0x3FF << 22) | (addrToPageDirIndex((uint32_t) ptr) << 12)))[addrToPageTableIndex((uint32_t) ptr)] & 0xFFFFF000; }
        static uint32_t allocPhysicalAndVirtualMemory(void **ptr, int len);
        static void changeRegionFlags(uint32_t virtAddr, uint32_t len, uint32_t setBits,
                                      uint32_t resetBits, uint32_t conditionMask = 0xFFFFFFFF, bool updatePageDirectory = true);
        inline static uint32_t pageTableEntry(uint32_t address, unsigned int flags) { return address | flags; }
        inline static uint32_t pageDirectoryEntry(uint32_t address, unsigned int flags) { return address | flags; }
        static void cleanUserspace();

        static void removePages(void *addr, unsigned long len);
   private:
        static void enable();
};

#endif
