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

inline int icmpHeaderAdditionalSize(int type)
{
    switch (type){
        case 3:
            return 4;

        default:
            return 0;
    }
}


void ICMP::processICMPPacket(NetIface *iface, uint8_t *packet, int size, void *previousHeader, int previousHeaderType)
{
    ICMPHeader *header = (ICMPHeader *) packet;
    IPHeader *ipHeader = (IPHeader *) previousHeader;

    int additionalSize = icmpHeaderAdditionalSize(header->type);
    if ((unsigned int) size < sizeof(ICMPHeader) + additionalSize){
        DEBUG_MSG("ICMP: packet size is smaller than ICMP header (size: %i).\n", size);
        return;
    }

    switch(header->type){
        case ICMP_ECHO_REPLY:
            DEBUG_MSG("PING REPLY\n");

        case ICMP_ECHO_REQUEST:
            DEBUG_MSG("PING REQUEST\n");

            if (ipHeader->daddr.addr == iface->myIP.addr){
                ICMP::sendICMPReply(iface, packet + sizeof(ICMPHeader), size - sizeof(ICMPHeader), ipHeader->daddr, ipHeader->saddr, ICMP_ECHO_REPLY, 0);
            }

            break;

        default:
            DEBUG_MSG("Unknown ICMP Packet type\n");

            break;
    }
}

void ICMP::sendICMPReply(NetIface *iface, uint8_t *data, int size, ipaddr srcIP, ipaddr destIP, int type, int code)
{
    int additionalSize = icmpHeaderAdditionalSize(type);

    int payloadOffset;
    uint8_t *newPacket = (uint8_t *) IP::allocPacketFor(data, sizeof(ICMPHeader) + additionalSize + size, srcIP, destIP, PROTOCOL_ICMP, &payloadOffset);

    ICMPHeader *newICMPHeader = (ICMPHeader *) (newPacket + payloadOffset);
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

    memcpy(newPacket + payloadOffset + sizeof(ICMPHeader) + additionalSize, data, size);
    newICMPHeader->checksum = checksum((uint16_t *) newICMPHeader, sizeof(ICMPHeader) + additionalSize + size);

    IP::sendTo(newPacket, sizeof(ICMPHeader) + additionalSize + size, srcIP, destIP, PROTOCOL_ICMP);
}
