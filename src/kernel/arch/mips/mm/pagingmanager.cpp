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

#include <core/printk.h>
#include <stdint.h>
#include <cstdlib.h>

#include <cp0registers.h>
#include <arch/mips/mm/physicalmm.h>

#define PDE_VALID 1

extern "C" void writeTLBRegisters(unsigned long pageMaskRegister, unsigned long entryHiRegister, unsigned long entryLo0REgister, unsigned long entryLo1Register);

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

inline void writePagesToRegisters(unsigned long pageMask, unsigned long virtualPageNumber, bool global, uint8_t asid,
                                     unsigned long physicalFrameNumber0, uint8_t cacheability0, bool dirty0, bool valid0,
                                     unsigned long physicalFrameNumber1, uint8_t cacheability1, bool dirty1, bool valid1)
{
    writeTLBRegisters(pageMask << 13, (virtualPageNumber << 13) | asid,
                      (physicalFrameNumber0 << 6) | (dirty0 << 2) | (valid0 << 1) | global,
                      (physicalFrameNumber1 << 6) | (dirty1 << 2) | (valid1 << 1) | global
                     );
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

    userPageDir[0] = pageDirEntry(PhysicalMM::allocPage(), true);
    kernelPageDir[511] = pageDirEntry(kseg0PtrToPhysicalAddress(kernelPageDir), true);
    physicalAddressToKSeg0Ptr<unsigned long *>(entryPhysicalAddress(userPageDir[0]))[2] = pageDirEntry(PhysicalMM::allocPage(), true);
}

extern "C" void tlbModificationExceptionISR(unsigned long address, unsigned long epc, unsigned long regX)
{
    printk("tlbModificationException: 0x%x (EPC: 0x%x), %x\n", address, epc, regX);
    while(1);
}

extern "C" void tlbLoadExceptionISR(unsigned long address, unsigned long epc, unsigned long regX)
{
    printk("tlbLoadException: 0x%x (EPC: 0x%x), %x\n", address, epc, regX);
    while(1);
}


extern "C" void tlbStoreExceptionISR(unsigned long address, unsigned long epc, unsigned long regX)
{
    printk("tlbStoreException: 0x%x (EPC: 0x%x), %x\n", address, epc, regX);
    while(1);
}
