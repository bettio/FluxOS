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
 *   Name: icmpv6.cpp                                                      *
 ***************************************************************************/

#include <net/net.h>

#include <net/netutils.h>
#include <net/ethernet.h>
#include <net/icmpv6.h>

#include <cstdlib.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

struct ICMPv6FakeHeader
{
    IPv6Addr saddr;
    IPv6Addr daddr;
    uint32_t len;
    uint16_t zeros0;
    uint8_t zeros1;
    uint8_t nextHeader;
};


void ICMPv6::processICMPv6Packet(NetIface *iface, uint8_t *packet, int size, void *previousHeader, int previousHeaderType)
{
    ICMPv6Header *header = (ICMPv6Header *) packet;
    IPv6Header *ipHeader = (IPv6Header *) previousHeader;

    switch(header->type){
        case ICMPV6_NDP_NEIGHBOR_SOLICITATION: {
            DEBUG_MSG("Neighbor Solicitation\n"); 
            NDPNeighborSolicitation *ns = (NDPNeighborSolicitation *) ((uint8_t *) header + sizeof(ICMPv6Header));
            NDPLinkLayerAddressOption *llo = (NDPLinkLayerAddressOption *) ((uint8_t *) ns + sizeof(NDPNeighborSolicitation));
            IPv6Addr *addr = new IPv6Addr;
            memcpy(addr, &ipHeader->saddr, sizeof(IPv6Addr));
            iface->macCache6.insert(addr, llo->linkLayerAddress);

            int payloadOffset;
            uint8_t *newPacket = (uint8_t *) IPv6::allocPacketFor(iface, 0, sizeof(ICMPv6Header) + sizeof(NDPNeighborAdvertisement) + sizeof(NDPLinkLayerAddressOption), &ipHeader->saddr, PROTOCOL_ICMPV6, &payloadOffset);
            
            ICMPv6Header *newHeader = (ICMPv6Header *) (newPacket + payloadOffset);
            newHeader->type = ICMPV6_NDP_NEIGHBOR_ADVERTISEMENT;
            newHeader->code = 0;
            newHeader->checksum = 0;

            NDPNeighborAdvertisement *adv = (NDPNeighborAdvertisement *) ((uint8_t *) newHeader + sizeof(ICMPv6Header));
            adv->flags = htonl(0x60000000);
            memcpy(&adv->targetAddress, &iface->myIP6, sizeof(IPv6Addr));

            NDPLinkLayerAddressOption *advOpt = (NDPLinkLayerAddressOption *) ((uint8_t *) adv + sizeof(NDPNeighborAdvertisement));
            advOpt->type = 2;
            advOpt->len = 1;
            advOpt->linkLayerAddress = iface->myMAC;

            ICMPv6FakeHeader icmpFake;
            memcpy(&icmpFake.saddr, &iface->myIP6, sizeof(IPv6Addr));
            memcpy(&icmpFake.daddr, &ipHeader->saddr, sizeof(IPv6Addr));
            icmpFake.zeros0 = 0;
            icmpFake.zeros1 = 0;
            icmpFake.nextHeader = 58;
            icmpFake.len = htonl(sizeof(ICMPv6Header) + sizeof(NDPNeighborAdvertisement) + sizeof(NDPLinkLayerAddressOption));
            newHeader->checksum = udpChecksum((uint16_t *) &icmpFake, sizeof(ICMPv6FakeHeader), (uint16_t *) newHeader, sizeof(ICMPv6Header) + sizeof(NDPNeighborAdvertisement) + sizeof(NDPLinkLayerAddressOption));

            IPv6::sendTo(iface, newPacket, sizeof(ICMPv6Header) + sizeof(NDPNeighborAdvertisement) + sizeof(NDPLinkLayerAddressOption), &ipHeader->saddr, PROTOCOL_ICMPV6);
        }
        break;

        case ICMPV6_ECHO_REQUEST: {
            DEBUG_MSG("PING6 REQUEST\n"); 
            ICMPv6Echo *echo = (ICMPv6Echo *) ((uint8_t *) header + sizeof(ICMPv6Header));

            int replyDataSize = ntohs(ipHeader->payloadLen) - (sizeof(ICMPv6Header) + sizeof(ICMPv6Echo));

            int payloadOffset;
            uint8_t *newPacket = (uint8_t *) IPv6::allocPacketFor(iface, 0, sizeof(ICMPv6Header) + sizeof(ICMPv6Echo) + replyDataSize, &ipHeader->saddr, PROTOCOL_ICMPV6, &payloadOffset);
            
            ICMPv6Header *newHeader = (ICMPv6Header *) (newPacket + payloadOffset);
            newHeader->type = ICMPV6_ECHO_REPLY;
            newHeader->code = 0;
            newHeader->checksum = 0;

            ICMPv6Echo *reply = (ICMPv6Echo *) ((uint8_t *) newHeader + sizeof(ICMPv6Header));
            reply->id = echo->id;
            reply->seqN = echo->seqN;

            memcpy((uint8_t *) reply + sizeof(ICMPv6Echo), (uint8_t *) echo + sizeof(ICMPv6Echo), replyDataSize);

            ICMPv6FakeHeader icmpFake;
            memcpy(&icmpFake.saddr, &iface->myIP6, sizeof(IPv6Addr));
            memcpy(&icmpFake.daddr, &ipHeader->saddr, sizeof(IPv6Addr));
            icmpFake.zeros0 = 0;
            icmpFake.zeros1 = 0;
            icmpFake.nextHeader = 58;
            icmpFake.len = htonl(sizeof(ICMPv6Header) + sizeof(ICMPv6Echo) + replyDataSize);
            newHeader->checksum = udpChecksum((uint16_t *) &icmpFake, sizeof(ICMPv6FakeHeader), (uint16_t *) newHeader, sizeof(ICMPv6Header) + sizeof(ICMPv6Echo) + replyDataSize);

            IPv6::sendTo(iface, newPacket, sizeof(ICMPv6Header) + sizeof(ICMPv6Echo) + replyDataSize, &ipHeader->saddr, PROTOCOL_ICMPV6);

        }
        break;

        default:
            DEBUG_MSG("Unknown ICMP Packet type: %i\n", header->type);

            break;
    }

}

