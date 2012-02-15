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
 *   Name: ip.cpp                                                          *
 ***************************************************************************/

#include <net/ip.h>

#include <net/icmp.h>
#include <net/net.h>
#include <net/netutils.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/ethernet.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

void IP::processIPPacket(NetIface *iface, uint8_t *packet, int size)
{
    IPHeader *header = (IPHeader *) packet;

#if 0
    if ((uint16_t) size < sizeof(IPHeader)){
        DEBUG_MSG("IP: packet length is smaller than min header size. (reported size: %i).\n", size);
        return;
    }
#endif
    if (header->version != 4){
        DEBUG_MSG("Not supported IP version.\n");
        return;
    }
    if ((unsigned int) header->ihl*4 < sizeof(IPHeader)){
        DEBUG_MSG("IP: packet header len is smaller than min header size. (size: %i).\n", header->ihl*4);
        return;
    }
#if 0
    if (ntohs(header->tot_len) > size){
        DEBUG_MSG("IP: discarded packet: wrong total len\n");
        return;
    }
#endif
    if (header->ihl*4 > ntohs(header->tot_len)){
        DEBUG_MSG("IP: discarded packet: packet header is bigger than packet\n");
        return;
    }

    switch(header->protocol){
        case PROTOCOL_ICMP:
            DEBUG_MSG("Net: ICMP packet\n");
            ICMP::processICMPPacket(iface, packet + header->ihl*4, ntohs(header->tot_len) - header->ihl*4);

            break;

        case PROTOCOL_TCP:
            DEBUG_MSG("Net: TCP packet\n");
            TCP::processTCPPacket(iface, packet + header->ihl*4, ntohs(header->tot_len) - header->ihl*4);

            break;

        case PROTOCOL_UDP:
            DEBUG_MSG("Net: UDP packet\n");
            UDP::processUDPPacket(iface, packet + header->ihl*4, ntohs(header->tot_len) - header->ihl*4);

            break;

        default:
            DEBUG_MSG("Unknown packet: protocol: %i\n", header->protocol);

            break;
    }
}

void IP::buildIPHeader(NetIface *iface, uint8_t *buffer, ipaddr destinationIP, uint8_t protocol, uint16_t dataLen)
{
    IPHeader *newIPHeader = (IPHeader *) buffer;
    newIPHeader->version = 4;
    newIPHeader->ihl = 5;
    newIPHeader->tos = 0;
    newIPHeader->tot_len = htons(dataLen);
    newIPHeader->id = 0;
    newIPHeader->frag_off = htons(0x4000);
    newIPHeader->ttl = 64;
    newIPHeader->protocol = protocol;
    newIPHeader->check = 0;
    newIPHeader->saddr = iface->myIP;
    newIPHeader->daddr = destinationIP;
    newIPHeader->check = checksum((uint16_t *) newIPHeader, sizeof(IPHeader));
}

void *IP::allocPacketFor(NetIface *iface, void *buf, int size, ipaddr destIP, int protocol, int *offset)
{
    macaddr macAddr = iface->macCache.value(destIP.addr);
    void *tmp = iface->allocPacketFor(iface, buf, size + sizeof(IPHeader), macAddr, ETHERTYPE_IP, offset);
    *offset += sizeof(IPHeader);

    return tmp;
}

void IP::sendTo(NetIface *iface, void *buf, int size, ipaddr destIP, int protocol)
{
    macaddr macAddr = iface->macCache.value(destIP.addr);
    buildIPHeader(iface, ((uint8_t *) buf) + sizeof(EthernetIIHeader), destIP, protocol, sizeof(IPHeader) + size);
    iface->sendTo(iface, buf, sizeof(IPHeader) + size, macAddr, ETHERTYPE_IP);
}

