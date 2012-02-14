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
#include <net/icmp.h>
#include <net/ip.h>
#include <net/arp.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

#define netBuffMalloc malloc

struct UDPFakeHeader
{
    ipaddr saddr;
    ipaddr daddr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t udpLen;
};

void UDP::processUDPPacket(NetIface *iface, uint8_t *packet, int size)
{
    IPHeader *ipHeader = (IPHeader *) (packet - sizeof(IPHeader)); //FIXME

    UDPHeader *header = (UDPHeader *) packet;
    ICMP::sendICMPReply(iface, (uint8_t *) ipHeader, size + sizeof(IPHeader), ipHeader->saddr, 3, 3);
    DEBUG_MSG("UDP Packet: SourcePort: %i, DestPort: %i, Len: %i\n", ntohs(header->sourceport), ntohs(header->destport), ntohs(header->length));
}

void UDP::sendTo(NetIface *iface, ipaddr destIp, uint8_t *packet, int size)
{
    int packetSize = sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(UDPHeader) + size;
    uint8_t *newPacket = (uint8_t *) netBuffMalloc(packetSize);

    macaddr macAddr = iface->macCache.value(destIp.addr);
    Ethernet::buildEthernetIIHeader(iface, newPacket, macAddr, ETHERTYPE_IP);
    IP::buildIPHeader(iface, newPacket + sizeof(EthernetIIHeader), destIp, PROTOCOL_UDP, sizeof(IPHeader) + sizeof(UDPHeader) + size);

    UDPHeader *newUDPHeader = (UDPHeader *) (newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader));
    newUDPHeader->sourceport = htons(555);
    newUDPHeader->destport = htons(89);
    newUDPHeader->length = htons(size + sizeof(UDPHeader));
    newUDPHeader->checksum = 0;

    memcpy(newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(UDPHeader), packet, size);

    UDPFakeHeader udpFake;
    udpFake.saddr = iface->myIP;
    udpFake.daddr = destIp;
    udpFake.zero = 0;
    udpFake.protocol = PROTOCOL_UDP;
    udpFake.udpLen = htons(sizeof(UDPHeader) + size);
    newUDPHeader->checksum = udpChecksum((uint16_t *) &udpFake, sizeof(udpFake), (uint16_t *) newUDPHeader, size + sizeof(UDPHeader));

    iface->send(iface, (const uint8_t *) newPacket, packetSize);
}

