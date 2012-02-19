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
#include <net/icmp.h>
#include <net/ip.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

struct UDPFakeHeader
{
    ipaddr saddr;
    ipaddr daddr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t udpLen;
};

void UDP::processUDPPacket(NetIface *iface, uint8_t *packet, int size, void *previousHeader, int previousHeaderType)
{
    IPHeader *ipHeader = (IPHeader *) previousHeader;

    UDPHeader *header = (UDPHeader *) packet;

    if ((ntohs(header->length) < sizeof(UDPHeader)) || ((uint16_t) size < sizeof(UDPHeader))){
        DEBUG_MSG("UDP: packet length is smaller than header size: %i, reported size: %i.\n", ntohs(header->length), size);
        return;
    }
    //TODO: should I discard every UDP packet with a different size?
    if (ntohs(header->length) > size){
        DEBUG_MSG("UDP: packet length is bigger than size reported from IP header: udp size: %i, reported size: %i.\n", ntohs(header->length), size);
        return;
    }
    if (header->destport == 0){
        DEBUG_MSG("UDP: packet destination port is set to 0.\n");
        return;
    }

    if (previousHeaderType == 4){
        ICMP::sendICMPReply(iface, (uint8_t *) ipHeader, size + sizeof(IPHeader), ipHeader->saddr, ICMP_UNREACHABLE, ICMP_UNREACHABLE_PORT);
    }

    DEBUG_MSG("UDP Packet: SourcePort: %i, DestPort: %i, Len: %i\n", ntohs(header->sourceport), ntohs(header->destport), ntohs(header->length));
}

void UDP::sendTo(NetIface *iface, ipaddr destIp, uint16_t srcPort, uint16_t destPort, uint8_t *packet, int size)
{
    int payloadOffset;
    uint8_t *newPacket = (uint8_t *) IP::allocPacketFor(iface, packet, sizeof(UDPHeader) + size, destIp, PROTOCOL_UDP, &payloadOffset);

    UDPHeader *newUDPHeader = (UDPHeader *) (newPacket + payloadOffset);
    newUDPHeader->sourceport = srcPort;
    newUDPHeader->destport = destPort;
    newUDPHeader->length = htons(size + sizeof(UDPHeader));
    newUDPHeader->checksum = 0;

    memcpy(newPacket + payloadOffset + sizeof(UDPHeader), packet, size);

    UDPFakeHeader udpFake;
    udpFake.saddr = iface->myIP;
    udpFake.daddr = destIp;
    udpFake.zero = 0;
    udpFake.protocol = PROTOCOL_UDP;
    udpFake.udpLen = htons(sizeof(UDPHeader) + size);
    newUDPHeader->checksum = udpChecksum((uint16_t *) &udpFake, sizeof(udpFake), (uint16_t *) newUDPHeader, size + sizeof(UDPHeader));

    IP::sendTo(iface, newPacket, sizeof(UDPHeader) + size, destIp, PROTOCOL_UDP);
}

