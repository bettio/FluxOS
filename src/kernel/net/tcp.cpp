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

#include <net/icmp.h>
#include <net/net.h>
#include <net/netutils.h>
#include <net/tcpsocket.h>
#include <task/eventsmanager.h>
#include <task/scheduler.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

QHash<uint16_t, VNode *> *TCP::openPorts;

void TCP::init()
{
    openPorts = new QHash<uint16_t, VNode *>;
}

void TCP::processTCPPacket(NetIface *iface, uint8_t *packet, int size, void *previousHeader, int previousHeaderType)
{
    IPHeader *ipHeader = (IPHeader *) previousHeader;

    TCPHeader *header = (TCPHeader *) packet;

    if (ntohs(header->flags) & TCP_FLAGS_SYN){
        DEBUG_MSG("TCP SYN\n");

        VNode *node = openPorts->value(header->destport);
        if (node == NULL){
            //Port closed, send ICMP reply
            if (previousHeaderType == 4){
                //TODO: check compiliancy
                ICMP::sendICMPReply(iface, (uint8_t *) ipHeader, size + sizeof(IPHeader), ipHeader->daddr, ipHeader->saddr, ICMP_UNREACHABLE, ICMP_UNREACHABLE_PORT);
            }
            return;
        }

        TCPSocket *mainSock = (TCPSocket *) node->privdata;
        TCPSocket *endPointSock = new TCPSocket;
        endPointSock->seqnumber = ntohl(header->seqnumber) + 1;
        if (mainSock->queuedConnections->count() >= mainSock->queuedConnections->capacity()){
             //Full queue
             return;
        }
        mainSock->queuedConnections->append(endPointSock);
        sendTCPPacket(iface, ipHeader->daddr, ipHeader->saddr, header->destport, header->sourceport, TCP_FLAGS_SYN | TCP_FLAGS_ACK, endPointSock->seqnumber, 10, 0, 0);

        ThreadControlBlock *thread = EventsManager::takeEventListener(node, EventsManager::NewDataAvail);
        if (thread != NULL){
            thread->status = Running;
        }

        return;
    }
    
    if (ntohs(header->flags) & TCP_FLAGS_FIN){
        DEBUG_MSG("FIN.\n");

        sendTCPPacket(iface, ipHeader->daddr, ipHeader->saddr, header->destport, header->sourceport, TCP_FLAGS_ACK | TCP_FLAGS_FIN, ntohl(header->seqnumber) + 1, ntohl(header->acknumber), 0, 0);
        return;
    }

    if (ntohs(header->flags) & TCP_FLAGS_ACK){
        int headerSize = (ntohs(header->flags) >> 12) * 4;
        DEBUG_MSG("ACK, size: %i\n", size - headerSize);

        if (size - headerSize){
            sendTCPPacket(iface, ipHeader->daddr, ipHeader->saddr, header->destport, header->sourceport, TCP_FLAGS_ACK, ntohl(header->seqnumber) + (size - headerSize), ntohl(header->acknumber), 0, 0);
        }
    }
}

void TCP::sendTCPPacket(NetIface *iface, ipaddr srcIP, ipaddr destIp, uint16_t srcPort, uint16_t destPort, int flags, int acknumber, int seqnumber, uint8_t *packet, int size)
{
    int payloadOffset;
    uint8_t *newPacket = (uint8_t *) IP::allocPacketFor(packet, sizeof(TCPHeader) + size, srcIP, destIp, PROTOCOL_TCP, &payloadOffset);

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
    newTCPHeader->checksum = IP::upperLayerChecksum(iface->myIP, destIp, PROTOCOL_TCP, newTCPHeader, sizeof(TCPHeader));

    IP::sendTo(newPacket, sizeof(TCPHeader) + size, srcIP, destIp, PROTOCOL_TCP);
}

#if 0
     UDPSocket *udpSock = (UDPSocket *) socknode->privdata;
     while (void *datagram = udpSock->datagrams->takeFirst()){
         UDPHeader *header = (UDPHeader *) datagram;
         void *data = (((uint8_t *) (datagram)) + sizeof(UDPHeader));
         int copySize = ((len - pos) > header->length) ? (len - pos) : header->length; //FIXME: min
         memcpy(((uint8_t *) buf) + pos, data, copySize);
         pos += copySize;
         if (pos == len) return 
     }
#endif

int TCP::bindVNodeToPort(uint16_t port, VNode *node)
{
    openPorts->insert(port, node);
    return 0;
}

