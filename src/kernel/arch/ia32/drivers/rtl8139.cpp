/***************************************************************************
 *   Copyright 2011 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: rtl8139.cpp                                                     *
 *   Date: 27/07/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/drivers/rtl8139.h>
#include <arch/ia32/io.h>
#include <core/printk.h>
#include <arch/ia32/core/pci.h>
#include <arch/ia32/core/irq.h>

#define RTL8139_INTRSTATUS 0x3E

#define RTL8139_RXACK 0x45

volatile uint8_t rx_buff[16384];
int ioBase;

bool rtl8139::init(int bus, int slot)
{
    ioBase = PCI::read(bus, slot, 0, PCI_IOBASE) & ~1;

    int irq = PCI::read(bus, slot, 0, 0x3C) & 0xF;
    IRQ::setHandler(rx, irq);
    IRQ::enableIRQ(irq);

    outportb(ioBase + 0x52, 0);
    outportb(ioBase + 0x37, 0x10);
    outport32(ioBase + 0x30, (uint32_t) rx_buff);
    outport16(ioBase + 0x3C, 0x0005);
    printk("rtl8139: RealTek 8139 card found (%x:%x). MAC address: %x:%x:%x:%x:%x:%x\n",
           bus, slot,
           inportb(ioBase + 0), inportb(ioBase + 1), inportb(ioBase + 2),
           inportb(ioBase + 3), inportb(ioBase + 4), inportb(ioBase + 5));
    outport32(ioBase + 0x44, 0xF | (1 << 7));
    outportb(ioBase + 0x37, 0x0C);
    
    return true;
}

void rtl8139::rx()
{
    printk("-packet-");
    for (int i = 0; i < 256; i++){
        printk("%x ", rx_buff[i]);
    }
    printk("-EOP-\n");
    outport16(ioBase + RTL8139_INTRSTATUS, RTL8139_RXACK);
}
