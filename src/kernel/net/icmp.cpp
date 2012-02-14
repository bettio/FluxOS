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
 *   Name: icmp.cpp                                                        *
 ***************************************************************************/

#include <net/net.h>

#include <net/netutils.h>
#include <net/ethernet.h>
#include <net/icmp.h>

#include <cstdlib.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

#define netBuffMalloc malloc

void ICMP::processICMPPacket(NetIface *iface, uint8_t *packet, int size)
{
    ICMPHeader *header = (ICMPHeader *) packet;
    IPHeader *ipHeader = (IPHeader *) (packet - sizeof(IPHeader)); //FIXME

    switch(header->type){
        case ECHO_REPLY:
            DEBUG_MSG("PING REPLY\n");

        case ECHO_REQUEST:
            DEBUG_MSG("PING REQUEST\n");

            if (ipHeader->daddr.addr == iface->myIP.addr){
                ICMP::sendICMPReply(iface, packet + sizeof(ICMPHeader), size - sizeof(ICMPHeader), ipHeader->saddr);
            }

            break;

        default:
            DEBUG_MSG("Unknown ICMP Packet type\n");

            break;
    }
}

void ICMP::sendICMPReply(NetIface *iface, uint8_t *data, int size, ipaddr destIp, int type, int code)
{
    int additionalSize = 0;
    switch (type){
        case 3:
            additionalSize = 4;
            break;

        default:
            additionalSize = 0;
    }

    uint8_t *newPacket = (uint8_t *) netBuffMalloc(sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader) + additionalSize + size);

    uint64_t macAddr = iface->macCache.value(destIp.addr);
    Ethernet::buildEthernetIIHeader(iface, newPacket, (uint8_t *) &macAddr, ETHERTYPE_IP);
    IP::buildIPHeader(iface, newPacket + sizeof(EthernetIIHeader), destIp, 0x01, sizeof(IPHeader) + sizeof(ICMPHeader) + additionalSize + size);

    ICMPHeader *newICMPHeader = (ICMPHeader *) (newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader));
    newICMPHeader->type = type;
    newICMPHeader->code = code;
    newICMPHeader->checksum = 0;

    switch (type){
        case 3:
            memset((uint8_t *) newICMPHeader + 4, 0, 4);
            break;

        default:
            additionalSize = 0;
    }

    memcpy(newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader) + additionalSize, data, size);
    newICMPHeader->checksum = checksum((uint16_t *) newICMPHeader, sizeof(ICMPHeader) + additionalSize + size);

    iface->send(iface, (const uint8_t *) newPacket, sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader) + additionalSize + size);
}
