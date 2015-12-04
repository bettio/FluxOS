/***************************************************************************
 *   Copyright 2105 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 23/10/2015                                                      *
 ***************************************************************************/

#include <arch/mips/mm/pagingmanager.h>

#define ENABLE_DEBUG_MSG 0
#include <debugmacros.h>

#include <core/printk.h>
#include <stdint.h>
#include <cstdlib.h>

#include <cp0registers.h>
#include <arch/mips/mm/physicalmm.h>
#include <mm/userspacememorymanager.h>
#include <mm/memorycontext.h>
#include <task/scheduler.h>

#define PDE_VALID 1
#define PTE_DIRTY 1

extern "C" void updateTLB(unsigned long *pageTable, int pageDirectoryIndex, int pageTableIndex, uint8_t asid);
extern "C" uint8_t switchASID(uint8_t asid);

unsigned long *kernelPageDir;
unsigned long *userPageDir;

template<typename T> inline T physicalAddressToKSeg0Ptr(unsigned long physicalAddr)
{
    return (T) (physicalAddr + 0x80000000);
}

inline unsigned long kseg0AddressToPhysicalAddress(unsigned long kseg0Addr)
{
    return kseg0Addr - 0x80000000;
}

inline unsigned long kseg0PtrToPhysicalAddress(const void *kseg0Ptr)
{
    return ((unsigned long) kseg0Ptr) - 0x80000000;
}

inline unsigned long pageDirEntry(unsigned long address, bool valid)
{
    return address | (valid ? PDE_VALID : 0);
}

inline unsigned long pageTableEntry(unsigned long address, bool valid)
{
    return address | (valid ? PDE_VALID : 0);
}

/*
 * Page dir entry
 * [0]: Valid
 * [1]: Write protect
 * [2-11]: Unused
 * [31-12]: Page phyisical address
 */

inline unsigned long entryPhysicalAddress(unsigned long entry)
{
    return entry & 0xFFFFFC00;
}

inline int addrToPageDirIndex(uint32_t addr)
{
    return addr >> 22;
}

inline int addrToPageTableIndex(uint32_t addr)
{
    return (addr >> 12) & 0x3FF;
}

inline int currentASID()
{
    return 0;
}

void PagingManager::init()
{
    //TODO: fix PhysicalMM code here
    PhysicalMM::init();
    for (uint32_t i = 0; i < 4096*1024; i++) {
        PhysicalMM::setAllocatedPage(i);
    }

    posix_memalign((void **) &kernelPageDir, 4096, 512*sizeof(long));
    memset(kernelPageDir, 0, 4096);
    posix_memalign((void **) &userPageDir, 4096, 512*sizeof(long));
    memset(userPageDir, 0, 4096);

    kernelPageDir[511] = pageDirEntry(kseg0PtrToPhysicalAddress(kernelPageDir), true);

}

AddressSpaceDescriptor PagingManager::switchAddressSpace(AddressSpaceDescriptor asDescriptor)
{
    userPageDir = asDescriptor.userPageDir;
    switchASID(asDescriptor.asid);
}

void PagingManager::removePages(void *addr, unsigned long len)
{
    printk("removePages\n");
    while(1);
}

void PagingManager::newPage(uint32_t addr, unsigned long flags)
{
    int di = addrToPageDirIndex(addr);
    int ti = addrToPageTableIndex(addr);

    DEBUG_MSG("PagingManager::newPage(0x%x, 0x%x): directory index: %i, table index: %i\n", newPage, flags, di, ti);

    if (userPageDir[di] == 0) {
        userPageDir[di] = pageDirEntry(PhysicalMM::allocPage(), true);
        memset(physicalAddressToKSeg0Ptr<unsigned long *>(entryPhysicalAddress(userPageDir[di])), 0, 4096);
    }

    unsigned long *pageTable = physicalAddressToKSeg0Ptr<unsigned long *>(entryPhysicalAddress(userPageDir[di]));
    pageTable[ti] = pageTableEntry(PhysicalMM::allocPage(), true);

    updateTLB(pageTable, di, ti, currentASID());
}

extern "C" void tlbModificationExceptionISR(unsigned long address, unsigned long epc, unsigned long regX)
{
    printk("tlbModificationException: 0x%x (EPC: 0x%x), %x\n", address, epc, regX);

    while(1);
}

extern "C" void tlbLoadExceptionISR(unsigned long faultAddress, unsigned long epc, unsigned long regX)
{
    MemoryContext *memoryContext = Scheduler::currentThread()->parentProcess->memoryContext;
    memoryContext->handlePageFault((void *) faultAddress, (void *) epc, UserspaceMemoryManager::ReadOperation, UserspaceMemoryManager::MissingPageFault);
}


extern "C" void tlbStoreExceptionISR(unsigned long faultAddress, unsigned long epc, unsigned long regX)
{
    MemoryContext *memoryContext = Scheduler::currentThread()->parentProcess->memoryContext;
    memoryContext->handlePageFault((void *) faultAddress, (void *) epc, UserspaceMemoryManager::WriteOperation, UserspaceMemoryManager::MissingPageFault);
}
