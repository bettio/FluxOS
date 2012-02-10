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
    mapPhysicalMemoryRegion(pageDir, 0, 0, alignToBound(kernel_heap_free_pos, 4096*1024)); 
    if (alignToBound(kernel_heap_free_pos, 4096*1024) != alignToBound(oldPos, 4096*1024)){
        printk("Warning: We didn't map everything");
	mapPhysicalMemoryRegion(pageDir, alignToBound(oldPos, 4096*1024), alignToBound(oldPos, 4096*1024), alignToBound(kernel_heap_free_pos, 4096*1024));
    }
    
    //Protect the first page against null pointer bugs (NULL_POINTERS_REGION)
    volatile uint32_t *firstPageTable = (volatile uint32_t *) (pageDir[0] & 0xFFFFF000);
    for (int i = 0; i < NULL_POINTERS_REGION_SIZE / PAGE_SIZE; i++){
        firstPageTable[i] = MISSING_PAGE;
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

volatile uint32_t *PagingManager::createPageDir()
{
    volatile uint32_t *pageDir = 0;
    posix_memalign((void **) &pageDir, PAGE_BOUNDARY, PAGE_SIZE);
    memset((void *) pageDir, 0, PAGE_SIZE);
    PagingManager::cloneKernelSpace(pageDir);

    return pageDir;
}

/*
 * This function is rather limited and it doesn't work in all situations
 */
void PagingManager::mapPhysicalMemoryRegion(volatile uint32_t *pageDir, uint32_t physAddr, uint32_t virtualAddr, uint32_t len)
{
    unsigned int physPages = 0;
    for (unsigned int i = addrToPageDirIndex(virtualAddr); i <= addrToPageDirIndex(virtualAddr + len - 1); i++){
        volatile uint32_t *pageTable;
        if (pageDir[i] == 0){
            pageTable = createEmptyPageTable();
            pageDir[i] = pageDirectoryEntry((uint32_t) pageTable, KERNEL_STD_PAGE);

        } else {
	    //HERE WE ASSUME THAT PAGING IS ALREADY ENABLED
	    //RECURSIVE PAGING
	    pageTable = (volatile uint32_t *) ((0x3FF << 22) | (i << 12));
            //TODO: IMPLEMENT ME
            //printk("PagingManager::mapMemoryRegion: implement me\n");
            //while (1);
        }

        int startAddr;
        if (addrToPageDirIndex(virtualAddr) < i){
            startAddr = i*4096*4096;
        }else{
            startAddr = virtualAddr;
        }
        int endAddr;
        if (addrToPageDirIndex(virtualAddr + len - 1) > i){
            endAddr = (i + 1)*4096*4096;
        }else{
            endAddr = virtualAddr + len;
        }
        for (unsigned int j = addrToPageTableIndex(startAddr); j <= addrToPageTableIndex(endAddr - 1); j++){ 
            pageTable[j] = pageTableEntry(physAddr + physPages*4096, KERNEL_STD_PAGE);
            PhysicalMM::setAllocatedPage(physAddr + physPages*4096);
            physPages++;
        }
    }
}

void *PagingManager::mapPhysicalMemory(uint32_t physAddr, int physLen)
{
    int len = physLen;

    void *virtMemPtr;
    if (posix_memalign((void **) &virtMemPtr, PAGE_BOUNDARY, len)){
        printk("Error: can't allocate %i bytes of virtual memory for %x", len, physAddr);
	return 0;
    }

    uint32_t virtualAddr = (uint32_t) virtMemPtr;
    volatile uint32_t *pageDir = (volatile uint32_t *) 0xFFFFF000;
    unsigned int physPages = 0;
    for (unsigned int i = addrToPageDirIndex(virtualAddr); i <= addrToPageDirIndex(virtualAddr + len - 1); i++){
        volatile uint32_t *pageTable;
        if (pageDir[i] == 0){
            pageTable = createEmptyPageTable();
            pageDir[i] = pageDirectoryEntry((uint32_t) pageTable, KERNEL_STD_PAGE);

        } else {
	    pageTable = (volatile uint32_t *) ((0x3FF << 22) | (i << 12));
        }

        int startAddr;
        if (addrToPageDirIndex(virtualAddr) < i){
            startAddr = i*4096*4096;
        }else{
            startAddr = virtualAddr;
        }
        int endAddr;
        if (addrToPageDirIndex(virtualAddr + len - 1) > i){
            endAddr = (i + 1)*4096*4096;
        }else{
            endAddr = virtualAddr + len;
        }
        for (unsigned int j = addrToPageTableIndex(startAddr); j <= addrToPageTableIndex(endAddr - 1); j++){ 
            pageTable[j] = pageTableEntry(physAddr + physPages*4096, KERNEL_STD_PAGE);
            PhysicalMM::setAllocatedPage(physAddr + physPages*4096);
            physPages++;
        }
    }

    return virtMemPtr;
}

void PagingManager::newPage(uint32_t addr)
{
   int di = addrToPageDirIndex(addr);
   int ti = addrToPageTableIndex(addr);

   volatile uint32_t *pageDir = (volatile uint32_t *) 0xFFFFF000;
   volatile uint32_t *pageTable = (volatile uint32_t *) ((0x3FF << 22) | (di << 12));

   if (pageDir[di] == MISSING_PAGE){
       pageDir[di] = pageDirectoryEntry(PhysicalMM::allocPage(), KERNEL_STD_PAGE);
       for (int i = 0; i < 1024; i++){
           pageTable[i] = MISSING_PAGE;
       }
   }

  pageTable[ti] = pageTableEntry(PhysicalMM::allocPage(), KERNEL_STD_PAGE);
}

void PagingManager::enable()
{
    volatile uint32_t cr0reg;
    asm volatile("movl %%cr0,%0": "=r"(cr0reg));
    cr0reg |= 0x80000000;
    asm volatile("movl %0, %%cr0\n": :"r"(cr0reg));
}

void PagingManager::cloneKernelSpace(volatile uint32_t *pageDir)
{
    volatile uint32_t *currentPageDir = (volatile uint32_t *) 0xFFFFF000; 

    for (int i = 0; i < KERNEL_SPACE_UPPER_LIMIT / (PAGE_SIZE*1024); i++){
        pageDir[i] = currentPageDir[i];
    }

    pageDir[1023] = pageDirectoryEntry((uint32_t) physicalAddressOf((void *) pageDir), KERNEL_STD_PAGE);
}

void PagingManager::changeAddressSpace(volatile uint32_t *pageDir, bool forceUpdate)
{
    cloneKernelSpace(pageDir);

    setCR3((uint32_t) physicalAddressOf((void *) pageDir));
}

#define hasMemoryPermissions(a, b) (1)
#define isMissingPageError(errorCode) ( !(errorCode & 1) )
#define GET_FAULT_EIP() *((uint32_t *) ((char *) &faultAddress + 32))

extern "C" void managePageFault(uint32_t faultAddress, uint32_t errorCode)
{
    //printk("Page Fault at 0x%x (error: %x)\n", faultAddress, errorCode);

    if (isMissingPageError(errorCode)){
        if (hasMemoryPermissions(faultAddress, errorCode)){
	    if (faultAddress < NULL_POINTERS_REGION_SIZE){
                const char *errorString = (errorCode & 2) ? "write" : "read";
                uint32_t eip = GET_FAULT_EIP();
                printk("Instruction at 0x%x tried to %s null pointer (addr: 0x%x)\n", eip, errorString, faultAddress);
		while (1);
	    }
	    PagingManager::newPage(faultAddress);
	}else{
            const char *errorString = (errorCode & 2) ? "write" : "read";
            printk("Segmentation Fault while trying to %s unallocated address 0x%x\n", errorString, faultAddress);
            while (1);
	}
    }else{
        const char *errorString = (errorCode & 2) ? "write" : "read";
        printk("Segmentation Fault while trying to %s address 0x%x\n", errorString, faultAddress);
        while (1);
    }
}

/*
 * If you change the stack usage, please make sure
 * to change also GET_FAULT_EIP
 */
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
