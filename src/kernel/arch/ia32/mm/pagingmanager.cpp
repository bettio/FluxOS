/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: pagingmanager.cpp                                               *
 *   Date: 23/09/2005                                                      *
 ***************************************************************************/

#include <arch/ia32/mm/pagingmanager.h>

#include <arch/ia32/mm/physicalmm.h>
#include <arch.h>
#include <arch/ia32/core/idt.h>
#include <core/printk.h>
#include <cstdlib.h>
#include <defs.h>

extern "C" void pageFaultHandler();

void PagingManager::init()
{
    IDT::setHandler(pageFaultHandler, 14);

    PhysicalMM::init();
    
    volatile uint32_t *pageDir = createEmptyPageDir();
    unsigned long oldPos = kernel_heap_free_pos;
    mapMemoryRegion(pageDir, 0, 0, alignToBound(kernel_heap_free_pos, 4096*1024)); 
    if (alignToBound(kernel_heap_free_pos, 4096*1024) != alignToBound(oldPos, 4096*1024)){
        printk("Warning: We didn't map everything");
	mapMemoryRegion(pageDir, alignToBound(oldPos, 4096*1024), alignToBound(oldPos, 4096*1024), alignToBound(kernel_heap_free_pos, 4096*1024));
    }
    
    setCR3((uint32_t) pageDir);
    enable();
}

volatile uint32_t *PagingManager::createEmptyPageDir()
{
    volatile uint32_t *pageDir;
    posix_memalign((void **) &pageDir, PAGE_BOUNDARY, PAGE_SIZE);
    for (int i = 0; i < PAGEDIR_ENTRIES; i++){
        pageDir[i] = MISSING_PAGE;
    }
    pageDir[1023] = pageDirectoryEntry((uint32_t) pageDir, KERNEL_STD_PAGE);

    return pageDir;
}

volatile uint32_t *PagingManager::createEmptyPageTable()
{
    volatile uint32_t *pageTable;
    posix_memalign((void **) &pageTable, PAGE_BOUNDARY, PAGE_SIZE);
    for (int k = 0; k < PAGETABLE_ENTRIES; k++){
        pageTable[k] = MISSING_PAGE;
    }
    return pageTable;
}

void PagingManager::mapMemoryRegion(volatile uint32_t *pageDir, uint32_t physAddr, uint32_t virtualAddr, uint32_t len)
{
    for (int i = addrToPageDirIndex(virtualAddr); i <= addrToPageDirIndex(virtualAddr + len - 1); i++){
        volatile uint32_t *pageTable;
        if (pageDir[i] == 0){
            pageTable = createEmptyPageTable();
            pageDir[i] = pageDirectoryEntry((uint32_t) pageTable, KERNEL_STD_PAGE);

        } else {
            //TODO: IMPLEMENT ME
        }
        for (int j = addrToPageTableIndex(virtualAddr); j <= addrToPageTableIndex(virtualAddr + len - 1); j++){
            pageTable[j] = pageTableEntry(physAddr + i*4096*4096 + j*4096, KERNEL_STD_PAGE);
            PhysicalMM::setAllocatedPage(physAddr + i*4096*4096 + j*4096);
        }
    }
}

void PagingManager::enable()
{
    volatile uint32_t cr0reg;
    asm volatile("movl %%cr0,%0": "=r"(cr0reg));
    cr0reg |= 0x80000000;
    asm volatile("movl %0, %%cr0\n": :"r"(cr0reg));
}

extern "C" void managePageFault(uint32_t faultAddress, uint32_t errorCode)
{
    printk("Page Fault at 0x%x (error: %x)", faultAddress, errorCode);
    while (1);
}

asm(".globl pageFaultHandler    \n"
    "pageFaultHandler:  \n"
    "nop                \n"
    "xchgl %eax,(%esp)  \n"
    "pushl %ecx         \n"
    "pushl %edx         \n"
    "push %ds           \n"
    "push %es           \n"
    "push %fs           \n"
    "movl $0x10,%edx    \n"
    "mov %dx,%ds        \n"
    "mov %dx,%es        \n"
    "mov %dx,%fs        \n"
    "movl %cr2,%edx     \n"
    "pushl %eax         \n"
    "pushl %edx         \n"
    "call managePageFault\n"
    "addl $8,%esp       \n"
    "pop %fs            \n"
    "pop %es            \n"
    "pop %ds            \n"
    "popl %edx          \n"
    "popl %ecx          \n"
    "popl %eax          \n"
    "iret               \n"
);
