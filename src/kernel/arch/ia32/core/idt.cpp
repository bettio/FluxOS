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
 *   Name: idt.cpp                                                         *
 *   Date: 28/12/2004                                                      *
 ***************************************************************************/

#include <arch/ia32/core/idt.h>
#include <arch/ia32/core/inthandler.h>
#include <stdint.h>

volatile uint64_t IDT::idt[IDT_SIZE];

void IDT::init()
{
    for(int i = 0; i < IDT_SIZE; i++){
        setHandler(IntHandler::IntNull, i);
    }

    setHandler(IntHandler::Int0, 0); //#DE - Divide Error
    setHandler(IntHandler::Int1, 1); //#DB - Debug
    setHandler(IntHandler::Int2, 2); //NMI Interrupt
    setHandler(IntHandler::Int3, 3); //#BP - Breakpoint
    setHandler(IntHandler::Int4, 4); //#OF - Overflow
    setHandler(IntHandler::Int5, 5); //#BR - BOUND Range Exceeded
    setHandler(IntHandler::Int6, 6); //#UD - Invalid Opcode
    setHandler(IntHandler::Int7, 7); //#NM - Device Not Avaible
    setHandler(IntHandler::Int8, 8); //#DF - Double Fault
    
    setHandler(IntHandler::Int10, 10); //#TS - Invalid TSS
    setHandler(IntHandler::Int11, 11); //#NP - Segment Not Present
    setHandler(IntHandler::Int12, 12); //#SS - Stack-Segment Fault
    setHandler(IntHandler::Int13, 13); //#GP - General Protection
    setHandler(IntHandler::Int14, 14); //#PF - Page Fault
    
    setHandler(IntHandler::Int16, 16); //#MF - Floating-Point Error
    setHandler(IntHandler::Int17, 17); //#AC - Alignment Check
    setHandler(IntHandler::Int18, 18); //#MC - Machine Check
    setHandler(IntHandler::Int19, 19); //#XF - Streaming SIMD Extensions

    setIDTR(idt, IDT_SIZE);
}

void IDT::setHandler(void (*func)(), int index, int dpl)
{
    uint32_t addr_func = (uint32_t)func;
    
    bool present = true;
    int ring = dpl;
    bool size32 = true;
    
    uint64_t tmp;
    
    tmp = addr_func & 0xFFFF;
    tmp |= (((uint64_t)addr_func) & 0x00000000FFFF0000LL) << 32;
    
    //Code segment
    tmp |= 0x8LL << 16;
    
    //Present
    if (present) tmp |=  0x0000800000000000LL;
    
    //Ring
    tmp |= (((uint64_t)ring) & 0x0000000000000003LL)<<44;
    
    //Size
    if(size32 == true){
        tmp |= 0x00000E0000000000LL;
    }else{
        tmp |= 0x0000060000000000LL;
    }
    
    idt[index] = tmp;
}

void IDT::setIDTR(volatile uint64_t *base, int numDesc)
{
    uint32_t idtReg[2];
    
    idtReg[0] = (numDesc*8) << 16;
    idtReg[1] = (uint32_t) base;
    asm volatile ("lidt (%0)": :"g" ((char *) idtReg + 2));
}
