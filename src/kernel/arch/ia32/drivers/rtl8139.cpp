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
#include <arch/ia32/mm/pagingmanager.h>
#include <arch/ia32/core/pci.h>
#include <arch/ia32/core/irq.h>
#include <net/net.h>
#include <net/netiface.h>
#include <net/ethernet.h>
#include <cstring.h>
#include <cstdlib.h>

#include <QList>

#define RTL8139_INTRSTATUS 0x3E

#define RTL8139_RXACK 0x45

QList<rtl8139 *> *cards;

bool rtl8139::init(int bus, int slot)
{
    if (cards == 0){
        cards = new QList<rtl8139 *>;
    }
    rtl8139 *newCard = new rtl8139;
    cards->append(newCard);

    newCard->ioBase = PCI::read(bus, slot, 0, PCI_IOBASE) & ~1;
    int ioBase = newCard->ioBase;

    int irq = PCI::read(bus, slot, 0, 0x3C) & 0xF;
    IRQ::setHandler(receive, irq);
    IRQ::enableIRQ(irq);

    uint32_t physRxBuf = PagingManager::allocPhysicalAndVirtualMemory((void **) &newCard->rx_buff, 8192 + 16 + 1500);
 
    outportb(ioBase + 0x52, 0);
    outportb(ioBase + 0x37, 0x10);
    outport32(ioBase + 0x30, physRxBuf);
    outport16(ioBase + 0x3C, 0xFFFF); //TODO: change 0xFFFF to something more specific otherwise some reserved bits are written
    printk("rtl8139: RealTek 8139 card found (%x:%x). MAC address: %x:%x:%x:%x:%x:%x\n",
           bus, slot,
           inportb(ioBase + 0), inportb(ioBase + 1), inportb(ioBase + 2),
           inportb(ioBase + 3), inportb(ioBase + 4), inportb(ioBase + 5));
    outport32(ioBase + 0x44, 0xF | (1 << 7));
    outportb(ioBase + 0x37, 0x0C);

    newCard->iface = new NetIface;
    newCard->iface->card = newCard;
    for (int i = 0; i < 6; i++){
        newCard->iface->myMAC[i] = inportb(ioBase + i);
    }
    newCard->iface->myIP.addrbytes[0] = 192;
    newCard->iface->myIP.addrbytes[1] = 168;
    newCard->iface->myIP.addrbytes[2] = 1;
    newCard->iface->myIP.addrbytes[3] = 5;
    newCard->iface->send = send;

    newCard->nextDesc = 0;

    return true;
}

void rtl8139::receive()
{
    for (int i = 0; i < cards->count(); i++){
        rtl8139 *card = cards->at(i);
        int ioBase = card->ioBase;
        int intStatus = inport16(ioBase + RTL8139_INTRSTATUS);
        if (intStatus & 1){
            uint16_t nextRxPtr = inport16(ioBase + 0x3A);

            outport16(ioBase + RTL8139_INTRSTATUS, 1);

            Ethernet::processEthernetIIFrame(card->iface, card->rx_buff + card->rxPtr + 4, *((uint16_t *) card->rx_buff + card->rxPtr + 2));

            card->rxPtr = nextRxPtr;
            outport16(ioBase + 0x38, card->rxPtr - 0x10);

        }else if (intStatus & 4){
            outport16(ioBase + RTL8139_INTRSTATUS, 0x04);

        }
    }
}

void rtl8139::send(NetIface *iface, const uint8_t *buff, unsigned int siz)
{
    rtl8139 *card = (rtl8139 *) iface->card;
    int ioBase = card->ioBase;

    while (1) {
        for (int i = card->nextDesc; i < 4; i++){
            if (inport32(ioBase + 0x10 + i*4) & (1 << 13)){
                card->nextDesc = (card->nextDesc + 1) % 4;
                outport32(ioBase + 0x20 + i*4, PagingManager::physicalAddressOf((void *) buff) + ((uint32_t) buff & 0xFFF));
                outport32(ioBase + 0x10 + i*4, siz); //size mask: 1FFF
                return;
            }
        }
    }
}
