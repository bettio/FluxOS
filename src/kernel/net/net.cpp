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
 *   Name: net.cpp                                                         *
 ***************************************************************************/

#include <net/net.h>

#include <net/arp.h>
#include <net/ethernet.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/udp.h>

#include <core/printk.h>
#include <cstdlib.h>
#include <stdint.h>
#include <cstring.h>

#include <endian.h>

#define IP_ADDRESS_LENGTH 4
#define MAC_ADDRESS_LENGTH 6

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

#define netBuffMalloc malloc

inline uint32_t htonl(uint32_t hostlong)
{
    return HOST_TO_BIG_32(hostlong);
}

inline int16_t htons(uint16_t hostshort)
{
    return HOST_TO_BIG_16(hostshort);
}

inline uint32_t ntohl(uint32_t netlong)
{
    return BIG_TO_HOST_32(netlong);
}

inline uint16_t ntohs(uint16_t netshort)
{
    return BIG_TO_HOST_16(netshort);
}

uint16_t checksum(uint16_t *data, int size)
{
    uint32_t sum = 0;

    for (int i = 0; i < size / 2; i++){
        sum += ntohs(data[i]);
    }

    return htons(~((sum & 0xFFFF) + (sum >> 16)));
}

void Net::setIface(NetIface *i)
{
    iface = i;
}

void Net::ProcessEthernetIIFrame(uint8_t *frame, int size)
{
    EthernetIIHeader *header = (EthernetIIHeader *) frame;

    switch(ntohs(header->type)){
        case ETHERTYPE_ARP:
              ProcessARPPacket(frame + sizeof(EthernetIIHeader), size - sizeof(EthernetIIHeader));

              break;

        case ETHERTYPE_IP:
              ProcessIPPacket(frame + sizeof(EthernetIIHeader), size - sizeof(EthernetIIHeader));

              break;

        case ETHERTYPE_IPV6:
              DEBUG_MSG("ProcessEthernetIIFrame: Unsupported IPv6 packet.\n");

              break;

        default:
              DEBUG_MSG("ProcessEthernetIIFrame: Unknown packet type: type: %x\n", ntohs(header->type));

              break;
    }
}

void Net::ProcessARPPacket(uint8_t *packet, int size)
{
    ARPPacket *arp = (ARPPacket *) packet;

    if ((arp->hardwareSize != MAC_ADDRESS_LENGTH) || (arp->protocolSize != IP_ADDRESS_LENGTH)){
        DEBUG_MSG("Unsupported address size: hardware: %i, protocol: %i\n", arp->hardwareSize, arp->protocolSize); 
    }

    memcpy(dummyMACCache, arp->senderMAC, 6);

    if ((arp->opcode == htons(ARP_OPCODE_REQUEST))){
        if (arp->targetIP == iface->myIP.addr){
            DEBUG_MSG("ARP request\n");
            SendARPReply(arp);
        }else{
            DEBUG_MSG("ARP request (to other)\n");
        }

    }else if (arp->opcode == htons(ARP_OPCODE_REPLY)){
        DEBUG_MSG("Received ARP reply\n");

    }else{
        DEBUG_MSG("Unknown ARP opcode: %i\n", arp->opcode);
    }
}

void Net::ProcessIPPacket(uint8_t *packet, int size)
{
    IPHeader *header = (IPHeader *) packet;

    if (header->version != 4){
        DEBUG_MSG("Not supported IP version.\n");
        return;
    }

    switch(header->protocol){
        case PROTOCOL_ICMP:
            DEBUG_MSG("Net: ICMP packet\n");
            ProcessICMPPacket(packet + header->ihl*4, size - header->ihl*4); //security

            break;

        case PROTOCOL_TCP:
            DEBUG_MSG("Net: TCP packet\n");
            ProcessTCPPacket(packet + header->ihl*4, size - header->ihl*4);

            break;

        case PROTOCOL_UDP:
            DEBUG_MSG("Net: UDP packet\n");
            ProcessUDPPacket(packet + header->ihl*4, size - header->ihl*4); //security

            break;

        default:
            DEBUG_MSG("Unknown packet: protocol: %i\n", header->protocol);

            break;
    }
}

void Net::ProcessICMPPacket(uint8_t *packet, int size)
{
    ICMPHeader *header = (ICMPHeader *) packet;
    IPHeader *ipHeader = (IPHeader *) (packet - sizeof(IPHeader)); //FIXME

    switch(header->type){
        case ECHO_REPLY:
            DEBUG_MSG("PING REPLY\n");

        case ECHO_REQUEST:
            DEBUG_MSG("PING REQUEST\n");

            if (ipHeader->daddr.addr == iface->myIP.addr){
                SendICMPReply(packet + sizeof(ICMPHeader), size - sizeof(ICMPHeader), ipHeader->saddr);
            }

            break;

        default:
            DEBUG_MSG("Unknown ICMP Packet type\n");

            break;
    }
}

void Net::ProcessUDPPacket(uint8_t *packet, int size)
{
    UDPHeader *header = (UDPHeader *) packet;

    DEBUG_MSG("UDP Packet: SourcePort: %i, DestPort: %i, Len: %i\n", ntohs(header->sourceport), ntohs(header->destport), ntohs(header->length));
}

void Net::ProcessTCPPacket(uint8_t *packet, int size)
{

}

void Net::BuildEthernetIIHeader(uint8_t *buffer, const uint8_t *destinationMAC, uint16_t type)
{
    EthernetIIHeader *newEth = (EthernetIIHeader *) buffer;
    memcpy(newEth->destination, destinationMAC, 6);
    memcpy(newEth->source, iface->myMAC, 6);
    newEth->type = htons(type);
}

void Net::BuildIPHeader(uint8_t *buffer, ipaddr destinationIP, uint8_t protocol, uint16_t dataLen)
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

void Net::SendARPReply(const ARPPacket *arpPacket)
{
    uint8_t *newPacket = (uint8_t *) netBuffMalloc(sizeof(EthernetIIHeader) + sizeof(ARPPacket));

    BuildEthernetIIHeader(newPacket, arpPacket->senderMAC, ETHERTYPE_ARP);

    ARPPacket *newArpPacket = (ARPPacket *) (newPacket + sizeof(EthernetIIHeader));
    newArpPacket->hardwareType = htons(1);
    newArpPacket->protocolType = htons(0x0800);
    newArpPacket->hardwareSize = 6;
    newArpPacket->protocolSize = 4;
    newArpPacket->opcode = htons(0x0002);
    memcpy(newArpPacket->senderMAC, iface->myMAC, 6);
    newArpPacket->senderIP = arpPacket->targetIP;
    memcpy(newArpPacket->targetMAC, arpPacket->senderMAC, 6);
    newArpPacket->targetIP = arpPacket->senderIP;

    iface->send(iface, newPacket, sizeof(EthernetIIHeader) + sizeof(ARPPacket));
}

void Net::SendICMPReply(uint8_t *data, int size, ipaddr destIp)
{
    uint8_t *newPacket = (uint8_t *) netBuffMalloc(sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader) + 80); //HARDCODED

    BuildEthernetIIHeader(newPacket, dummyMACCache, ETHERTYPE_IP);
    BuildIPHeader(newPacket + sizeof(EthernetIIHeader), destIp, 0x01, 0x54);

    ICMPHeader *newICMPHeader = (ICMPHeader *) (newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader));
    newICMPHeader->type = 0;
    newICMPHeader->code = 0;
    newICMPHeader->checksum = 0;
    memcpy(newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader), data, 60);
    newICMPHeader->checksum = checksum((uint16_t *) newICMPHeader, 64);

    iface->send(iface, (const uint8_t *) newPacket, sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader) + 60);
}
