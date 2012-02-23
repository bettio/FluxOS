/***************************************************************************
 *   Copyright 2012 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: ipv6.cpp                                                        *
 ***************************************************************************/

#include <net/ipv6.h>
#include <net/ip.h>

#include <net/ethernet.h>
#include <net/net.h>
#include <net/netiface.h>
#include <net/netutils.h>
#include <net/icmpv6.h>
#include <net/tcp.h>
#include <net/udp.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

void IPv6::init()
{
}

void ipv6toString(IPv6Addr addr, char *str)
{
    int l = 0;
    for (int i = 0; i < 16; i++){
        if (addr.addrbytes[i] <= 0xF){
            str[l] = '0';
            l++;
        }
        l += uitoaz(addr.addrbytes[i], &str[l], 16);
        if (i != 15 && (i % 2)){
            str[l] = ':';
            l++;
        }
    }
}

void IPv6::processIPv6Packet(NetIface *iface, uint8_t *packet, int size)
{
    IPv6Header *header = (IPv6Header *) packet;

    char ip[48];
    ipv6toString(header->saddr, ip);
    DEBUG_MSG("processIPv6Packet from: %s size: %i\n", ip, size);

    int payloadOffset = sizeof(IPv6Header);

    switch(header->nextHeader){
        case PROTOCOL_ICMPV6:
            DEBUG_MSG("IPv6: ICMP packet\n");
            ICMPv6::processICMPv6Packet(iface, packet + payloadOffset, header->payloadLen, header, 6);

            break;

        case PROTOCOL_TCP:
            DEBUG_MSG("IPv6: TCP packet\n");
            TCP::processTCPPacket(iface, packet + payloadOffset, header->payloadLen, header, 6);

            break;

        case PROTOCOL_UDP:
            DEBUG_MSG("IPv6: UDP packet\n");
            UDP::processUDPPacket(iface, packet + payloadOffset, header->payloadLen, header, 6);

            break;

        default:
            DEBUG_MSG("Unknown packet: protocol: %i\n", header->nextHeader);

            break;
    }
}

void IPv6::buildIPv6Header(NetIface *iface, uint8_t *buffer, IPv6Addr *destinationIP, uint8_t protocol, uint16_t dataLen)
{
    IPv6Header *newIPHeader = (IPv6Header *) buffer;
    newIPHeader->head = htonl(0x60000000);
    newIPHeader->payloadLen = ntohs(dataLen);
    newIPHeader->nextHeader = protocol;
    newIPHeader->hopLimit = 255;
    memcpy(&newIPHeader->daddr, destinationIP, sizeof(IPv6Addr));
    memcpy(&newIPHeader->saddr, &iface->myIP6, sizeof(IPv6Addr));
}

uint16_t IPv6::upperLayerChecksum(IPv6Addr *saddr, IPv6Addr *daddr, int protocol, void *header, int size)
{
    IPv6FakeHeader ipFake;
    memcpy(&ipFake.saddr, saddr, sizeof(IPv6Addr));
    memcpy(&ipFake.daddr, daddr, sizeof(IPv6Addr));
    ipFake.zeros0 = 0;
    ipFake.zeros1 = 0;
    ipFake.nextHeader = protocol;
    ipFake.len = htonl(size);
    return checksum((uint16_t *) &ipFake, sizeof(IPv6FakeHeader), (uint16_t *) header, size);
}

void *IPv6::allocPacketFor(NetIface *iface, void *buf, int size, IPv6Addr *destIP, int protocol, int *offset)
{
    macaddr macAddr = iface->macCache6.value(destIP);
    void *tmp = iface->allocPacketFor(iface, buf, size + sizeof(IPv6Header), macAddr, ETHERTYPE_IPV6, offset);
    *offset += sizeof(IPv6Header);

    return tmp;
}

void IPv6::sendTo(NetIface *iface, void *buf, int size, IPv6Addr *destIP, int protocol)
{
    macaddr macAddr = iface->macCache6.value(destIP);
    buildIPv6Header(iface, ((uint8_t *) buf) + sizeof(EthernetIIHeader), destIP, protocol, size);
    iface->sendTo(iface, buf, sizeof(IPv6Header) + size, macAddr, ETHERTYPE_IPV6);
}

