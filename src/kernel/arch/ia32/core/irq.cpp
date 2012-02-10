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
 *   Name: irq.cpp                                                         *
 *   Date: 30/12/2004                                                      *
 ***************************************************************************/

#include <arch/ia32/core/irq.h>
#include <arch/ia32/core/idt.h>
#include <arch/ia32/core/contextswitcher.h>
#include <bitutils.h>
#include <arch/ia32/io.h>
#include <core/printk.h>

#define M_PIC	0x20
#define S_PIC	0xA0
#define M_VEC	0x30
#define S_VEC	(M_VEC + 8)
#define ICW1	0x11
#define ICW2_M	M_VEC
#define ICW2_S	S_VEC	
#define ICW3_M	(1 << 2)
#define ICW3_S	3
#define ICW4	0x01

uint16_t IRQ::irqMask;
void (*IRQ::handler[16])();

extern "C" void irqHandler();

void IRQ::init()
{
    init8259();

    for (int i = 0; i < 16; i++) {
        if (i < 8){
            IDT::setHandler(irqHandler, M_VEC + i);
        }else{
            IDT::setHandler(irqHandler, S_VEC + i - 8);
        }
        setHandler(nullHandler, i);
    }
    
    ContextSwitcher::init();
}

void IRQ::setHandler(void (*func) (), uint8_t irq)
{
    handler[irq] = func;
}

void IRQ::enableIRQ(int irq_no)
{
    irqMask &= ~(1 << irq_no);

    if (irq_no >= 8){
        irqMask &= ~(1 << 2);
    }

    outportb_p(M_PIC + 1, irqMask & 0xFF);
    outportb_p(S_PIC + 1, (irqMask >> 8) & 0xFF);
}

void IRQ::disableIRQ(int irq_no)
{
    irqMask |= (1 << irq_no);

    if ((irqMask & 0xFF00) == 0xFF00){
        irqMask |= (1 << 2);
    }

    outportb_p(M_PIC + 1, irqMask & 0xFF);
    outportb_p(S_PIC + 1, (irqMask >> 8) & 0xFF);
}

void IRQ::init8259()
{
    outportb_p(M_PIC, ICW1);
    outportb_p(S_PIC, ICW1);

    outportb_p(M_PIC + 1, ICW2_M);
    outportb_p(S_PIC + 1, ICW2_S);

    outportb_p(M_PIC + 1, ICW3_M);
    outportb_p(S_PIC + 1, ICW3_S);

    outportb_p(M_PIC + 1, ICW4);
    outportb_p(S_PIC + 1, ICW4);

    irqMask = 0xFFFF;
    outportb_p(M_PIC + 1, irqMask & 0xFF);
    outportb_p(S_PIC + 1, (irqMask >> 8) & 0xFF);
}

void IRQ::nullHandler()
{
    printk("irq: null handler. IRQ: %i\n", currentIRQ());
}

int IRQ::currentIRQ()
{
    uint8_t irq;

    outportb_p(M_PIC, 0x0B);
    irq = inportb_p(M_PIC);

    if (irq == 0x04) {
        outportb_p(S_PIC, 0x0B);
        irq = inportb_p(S_PIC);
        return firstOneIndex8(irq) + 8;
     }
        
     return firstOneIndex8(irq);
}

void IRQ::endOfIRQ(int irq)
{
    if (irq >= 8){
        outportb(S_PIC, 0x20);
    }

    outportb(M_PIC, 0x20);
}

extern "C"
{
    void doIRQ(uint32_t esp)
    {
        uint8_t irq = IRQ::currentIRQ();

        (*IRQ::handler[irq])();

        ContextSwitcher::schedule((long *) &esp);
        
        IRQ::endOfIRQ(irq);
    }

    asm(".globl irqHandler\n"
        "irqHandler:\n"
        "cli\n"
        "pusha\n"
        "movl %esp, %eax\n"
        "pushl %eax\n"
        "call doIRQ\n"
        "popl %eax\n"
        "mov %eax, %esp\n"
        "popa\n"
        "sti\n"
        "iret\n"
    );
}
