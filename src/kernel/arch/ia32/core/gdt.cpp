/***************************************************************************
 *   Copyright 2004 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: gdt.cpp                                                         *
 *   Date: 10/12/2004                                                      *
 ***************************************************************************/

#include <arch/ia32/core/gdt.h>
#include <arch/ia32/core/tss.h>

volatile uint64_t GDT::gdt[GDT_SIZE];

void GDT::init()
{
    //Always required
    gdt[0] = 0;
    //R/X code segment
    gdt[1] = 0x00CF9A000000FFFFLL;
    //R/W data segment
    gdt[2] = 0x00CF92000000FFFFLL;
    //R/X code segment
    gdt[3] = 0x00CFFA000000FFFFLL;
    //R/W data segment
    gdt[4] = 0x00CFF2000000FFFFLL;
    //Update GDTR
    setGDTR(gdt, GDT_SIZE);
    
    asm volatile("pushl $2; popf" : : : "cc");

    asm volatile ("movw $0x10, %%ax\n"
              "movw %%ax, %%ds\n"
              "movw %%ax, %%es\n"
              "movw %%ax, %%fs\n"
              "movw %%ax, %%gs\n"
              "movw %%ax, %%ss\n"
              "ljmp $0x08, $next\n"
              "nop\n"
              "nop\n"
              "next:\n"
               : : : "%ax"
    );

    TSS::init(gdt + 5);
    TSS::ltr(5);
}

void GDT::setGDTR(volatile uint64_t *base, int numDesc)
{
    volatile uint32_t gdtReg[2];

    gdtReg[0] = (numDesc * 8) << 16;
    gdtReg[1] = (uint32_t) base;

    asm volatile("lgdt (%0)" : : "g"((volatile char *) gdtReg + 2));
}
