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

#include <filesystem/vnode.h>
#include <net/netutils.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/udpsocket.h>
#include <task/eventsmanager.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

QHash<uint16_t, VNode *> *UDP::openPorts;

void UDP::init()
{
    openPorts = new QHash<uint16_t, VNode *>;
}

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

    DEBUG_MSG("UDP Packet: SourcePort: %i, DestPort: %i, Len: %i\n", ntohs(header->sourceport), ntohs(header->destport), ntohs(header->length));

    //Ready to deliver packet content
    VNode *node = openPorts->value(header->destport);
    if (node == NULL){
        //Connection closed, send ICMP reply
        if (previousHeaderType == 4){
            //TODO: check compiliancy
            ICMP::sendICMPReply(iface, (uint8_t *) ipHeader, size + sizeof(IPHeader), ipHeader->daddr, ipHeader->saddr, ICMP_UNREACHABLE, ICMP_UNREACHABLE_PORT);
        }
        return;
    }

    void *dataBuff = malloc(header->length);
    memcpy(dataBuff, packet, header->length);
    UDPSocket *sock = (UDPSocket *) node->privdata;
    sock->datagrams->append(dataBuff);

    ThreadControlBlock *thread = EventsManager::takeEventListener(node, EventsManager::NewDataAvail);
    if (thread != NULL){
        thread->status = Running;
    }
}

void UDP::sendTo(NetIface *iface, ipaddr srcIP, ipaddr destIp, uint16_t srcPort, uint16_t destPort, uint8_t *packet, int size)
{
    int payloadOffset;
    uint8_t *newPacket = (uint8_t *) IP::allocPacketFor(packet, sizeof(UDPHeader) + size, srcIP, destIp, PROTOCOL_UDP, &payloadOffset);

    UDPHeader *newUDPHeader = (UDPHeader *) (newPacket + payloadOffset);
    newUDPHeader->sourceport = srcPort;
    newUDPHeader->destport = destPort;
    newUDPHeader->length = htons(size + sizeof(UDPHeader));
    newUDPHeader->checksum = 0;
    
    memcpy(newPacket + payloadOffset + sizeof(UDPHeader), packet, size);

    newUDPHeader->checksum = IP::upperLayerChecksum(iface->myIP, destIp, PROTOCOL_UDP, newUDPHeader, size + sizeof(UDPHeader));

    IP::sendTo(newPacket, sizeof(UDPHeader) + size, srcIP, destIp, PROTOCOL_UDP);
}

int UDP::bindVNodeToPort(uint16_t port, VNode *node)
{
    openPorts->insert(port, node);
    return 0;
}

