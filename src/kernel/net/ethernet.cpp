/***************************************************************************
 *   Copyright 2009 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: ethernet.cpp                                                         *
 ***************************************************************************/

#include <net/net.h>

#include <net/netutils.h>
#include <net/arp.h>
#include <net/ethernet.h>
#include <net/ip.h>

#include <cstdlib.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

#include <net/ethernet.h>
#include <net/netutils.h>

void *Ethernet::allocPacketFor(NetIface *iface, void *buf, int size, macaddr destMAC, int protocol, int *offset)
{
    void *newPacket = malloc(sizeof(EthernetIIHeader) + size);
    *offset = sizeof(EthernetIIHeader);

    return newPacket;
}

void Ethernet::sendTo(NetIface *iface, void *buf, int size, macaddr destMAC, int protocol)
{
    Ethernet::buildEthernetIIHeader(iface, (uint8_t *) buf, destMAC, protocol);
    iface->send(iface, (uint8_t *) buf, sizeof(EthernetIIHeader) + size);
}

void Ethernet::processEthernetIIFrame(NetIface *iface, uint8_t *frame, int size)
{
    EthernetIIHeader *header = (EthernetIIHeader *) frame;

    switch(ntohs(header->type)){
        case ETHERTYPE_ARP:
              ARP::processARPPacket(iface, frame + sizeof(EthernetIIHeader), size - sizeof(EthernetIIHeader));

              break;

        case ETHERTYPE_IP:
              IP::processIPPacket(iface, frame + sizeof(EthernetIIHeader), size - sizeof(EthernetIIHeader));

              break;

        case ETHERTYPE_IPV6:
              DEBUG_MSG("ProcessEthernetIIFrame: Unsupported IPv6 packet.\n");

              break;

        default:
              DEBUG_MSG("ProcessEthernetIIFrame: Unknown packet type: type: %x\n", ntohs(header->type));

              break;
    }
}

void Ethernet::buildEthernetIIHeader(NetIface *iface, uint8_t *buffer, macaddr destinationMAC, uint16_t type)
{
    EthernetIIHeader *newEth = (EthernetIIHeader *) buffer;
    newEth->destination = destinationMAC;
    newEth->source = iface->myMAC;
    newEth->type = htons(type);
}
