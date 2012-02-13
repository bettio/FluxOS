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
 *   Name: udp.cpp                                                         *
 ***************************************************************************/

#include <net/net.h>

#include <net/netutils.h>
#include <net/udp.h>
#include <net/ethernet.h>
#include <net/ip.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

#define netBuffMalloc malloc

void UDP::processUDPPacket(NetIface *iface, uint8_t *packet, int size)
{
    UDPHeader *header = (UDPHeader *) packet;

    DEBUG_MSG("UDP Packet: SourcePort: %i, DestPort: %i, Len: %i\n", ntohs(header->sourceport), ntohs(header->destport), ntohs(header->length));
}

void UDP::sendTo(NetIface *iface, ipaddr destIp, uint8_t *packet, int size)
{
    uint8_t *newPacket = (uint8_t *) netBuffMalloc(sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(UDPHeader) + size);

    uint64_t macAddr = iface->macCache.value(destIp.addr);
    Ethernet::buildEthernetIIHeader(iface, newPacket, (uint8_t *) &macAddr, ETHERTYPE_IP);
    IP::buildIPHeader(iface, newPacket + sizeof(EthernetIIHeader), destIp, PROTOCOL_UDP, sizeof(IPHeader) + sizeof(UDPHeader) + size);

    UDPHeader *newUDPHeader = (UDPHeader *) (newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader));
    newUDPHeader->sourceport = htons(555);
    newUDPHeader->destport = htons(89);
    newUDPHeader->length = htons(size + sizeof(UDPHeader));
    newUDPHeader->checksum = 0;
    memcpy(newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(UDPHeader), packet, size);

    iface->send(iface, (const uint8_t *) newPacket, sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(UDPHeader) + size);
}

