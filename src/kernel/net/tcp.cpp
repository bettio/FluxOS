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
 *   Name: tcp.cpp                                                         *
 ***************************************************************************/

#include <net/tcp.h>

#include <net/net.h>
#include <net/netutils.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

struct TCPFakeHeader
{
    ipaddr saddr;
    ipaddr daddr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t tcpLen;
};

uint16_t checksum(ipaddr saddr, ipaddr daddr, void *header, int size)
{
    TCPFakeHeader tcpFake;
    tcpFake.saddr = saddr;
    tcpFake.daddr = daddr;
    tcpFake.zero = 0;
    tcpFake.protocol = PROTOCOL_TCP;
    tcpFake.tcpLen = htons(size);
    return udpChecksum((uint16_t *) &tcpFake, sizeof(tcpFake), (uint16_t *) header, size);
}

void TCP::init()
{

}

void TCP::processTCPPacket(NetIface *iface, uint8_t *packet, int size, void *previousHeader, int previousHeaderType)
{
    IPHeader *ipHeader = (IPHeader *) previousHeader;

    TCPHeader *header = (TCPHeader *) packet;

    if (ntohs(header->flags) & TCP_FLAGS_SYN){
        DEBUG_MSG("TCP SYN\n");
        sendTCPPacket(iface, ipHeader->saddr, header->destport, header->sourceport, TCP_FLAGS_SYN | TCP_FLAGS_ACK, ntohl(header->seqnumber) + 1, 10, 0, 0);
        return;
    }
    
    if (ntohs(header->flags) & TCP_FLAGS_FIN){
        DEBUG_MSG("FIN.\n");

        sendTCPPacket(iface, ipHeader->saddr, header->destport, header->sourceport, TCP_FLAGS_ACK | TCP_FLAGS_FIN, ntohl(header->seqnumber) + 1, ntohl(header->acknumber), 0, 0);
        return;
    }

    if (ntohs(header->flags) & TCP_FLAGS_ACK){
        int headerSize = (ntohs(header->flags) >> 12) * 4;
        DEBUG_MSG("ACK, size: %i\n", size - headerSize);

        if (size - headerSize){
            sendTCPPacket(iface, ipHeader->saddr, header->destport, header->sourceport, TCP_FLAGS_ACK, ntohl(header->seqnumber) + (size - headerSize), ntohl(header->acknumber), 0, 0);
        }
    }
}

void TCP::sendTCPPacket(NetIface *iface, ipaddr destIp, uint16_t srcPort, uint16_t destPort, int flags, int acknumber, int seqnumber, uint8_t *packet, int size)
{
    int payloadOffset;
    uint8_t *newPacket = (uint8_t *) IP::allocPacketFor(iface, packet, sizeof(TCPHeader) + size, destIp, PROTOCOL_TCP, &payloadOffset);

    memcpy(newPacket + payloadOffset + sizeof(TCPHeader), packet, size);

    TCPHeader *newTCPHeader = (TCPHeader *) (newPacket + payloadOffset);  
    newTCPHeader->sourceport = srcPort;
    newTCPHeader->destport = destPort;
    newTCPHeader->acknumber = htonl(acknumber);
    newTCPHeader->seqnumber = htonl(seqnumber);
    newTCPHeader->flags = htons(0x5000 | flags);
    newTCPHeader->windowSize = htons(256);
    newTCPHeader->checksum = 0;
    newTCPHeader->urgentPtr = 0;
    newTCPHeader->checksum = checksum(iface->myIP, destIp, newTCPHeader, sizeof(TCPHeader));

    IP::sendTo(iface, newPacket, sizeof(TCPHeader) + size, destIp, PROTOCOL_TCP);
}

