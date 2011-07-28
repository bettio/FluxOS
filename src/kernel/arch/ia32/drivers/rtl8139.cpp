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
#include <net/net.h>
#include <cstring.h>
#include <cstdlib.h>
#define RTL8139_INTRSTATUS 0x3E

#define RTL8139_RXACK 0x45

uint8_t *rx_buff;
uint16_t rxPtr;
int ioBase;
Net *net;

bool rtl8139::init(int bus, int slot)
{
    ioBase = PCI::read(bus, slot, 0, PCI_IOBASE) & ~1;

    int irq = PCI::read(bus, slot, 0, 0x3C) & 0xF;
    IRQ::setHandler(receive, irq);
    IRQ::enableIRQ(irq);

    rx_buff = (uint8_t *) malloc(8192 + 16 + 1500);
    
    outportb(ioBase + 0x52, 0);
    outportb(ioBase + 0x37, 0x10);
    outport32(ioBase + 0x30, (uint32_t) rx_buff);
    outport16(ioBase + 0x3C, 0xFFFF); //TODO: change 0xFFFF to something more specific otherwise some reserved bits are written
    printk("rtl8139: RealTek 8139 card found (%x:%x). MAC address: %x:%x:%x:%x:%x:%x\n",
           bus, slot,
           inportb(ioBase + 0), inportb(ioBase + 1), inportb(ioBase + 2),
           inportb(ioBase + 3), inportb(ioBase + 4), inportb(ioBase + 5));
    outport32(ioBase + 0x44, 0xF | (1 << 7));
    outportb(ioBase + 0x37, 0x0C);
    
    NetIface *iface = new NetIface;
    for (int i = 0; i < 6; i++){
        iface->myMAC[i] = inportb(ioBase + i);
    }
    iface->myIP.addrbytes[0] = 10;
    iface->myIP.addrbytes[1] = 0;
    iface->myIP.addrbytes[2] = 0;
    iface->myIP.addrbytes[3] = 5;
    iface->send = send;
    net = new Net;
    net->setIface(iface);

    return true;
}

void rtl8139::receive()
{
    int intStatus = inport16(ioBase + RTL8139_INTRSTATUS);
    if (intStatus & 1){
        uint16_t nextRxPtr = inport16(ioBase + 0x3A);
        
        outport16(ioBase + RTL8139_INTRSTATUS, 1);

        net->ProcessEthernetIIFrame(rx_buff + rxPtr + 4, *((uint16_t *) rx_buff + rxPtr + 2));
        
        rxPtr = nextRxPtr;
        outport16(ioBase + 0x38, rxPtr - 0x10);

    }else if (intStatus & 4){
        outport16(ioBase + RTL8139_INTRSTATUS, 0x04);

    }else{
        printk("rtl8139: error: unsupported interrupt status\n");
        while(1);
    }
}
    
void rtl8139::send(const uint8_t *buff, unsigned int siz)
{
    while (1) {
        for (int i = 0; i < 4; i++){
            if (inport32(ioBase + 0x10 + i*4) & (1 << 13)){
                outport32(ioBase + 0x20 + i*4, (uint32_t) buff);
                outport32(ioBase + 0x10 + i*4, siz);
                return;
            }
        }
    }
}
