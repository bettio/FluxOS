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

#include <core/printk.h>
#include <cstdlib.h>
#include <stdint.h>
#include <cstring.h>

#include <endian.h>

#define PROTOCOL_ICMP 1
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 17

#define ECHO_REPLY 0
#define ECHO_REQUEST 8

#define ETHERNETII_TYPE_IP 0x0800
#define ETHERNETII_TYPE_ARP 0x0806
#define ETHERNETII_TYPE_IPV6 0x86DD

#define ARP_OPCODE_REQUEST 0x0001
#define ARP_OPCODE_REPLY 0x0002

#define IP_PROTOCOL_ICMP 0x01
#define IP_PROTOCOL_UDP 0x11


#define IP_ADDRESS_LENGTH 4
#define MAC_ADDRESS_LENGTH 6

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

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

struct EthernetIIHeader
{
  uint8_t destination[6];
  uint8_t source[6];
  uint16_t type;
} __attribute__ ((packed));;

struct ARPPacket
{
  uint16_t hardwareType;
  uint16_t protocolType;
  uint8_t hardwareSize;
  uint8_t protocolSize;
  uint16_t opcode;
  uint8_t senderMAC[6];
  uint32_t senderIP;
  uint8_t targetMAC[6];
  uint32_t targetIP;
} __attribute__ ((packed));

struct IPHeader {
   uint8_t   ihl:4, version:4;
   uint8_t      tos;
   uint16_t     tot_len;
   uint16_t     id;
   uint16_t     frag_off;
   uint8_t      ttl;
   uint8_t      protocol;
   uint16_t     check;
   ipaddr       saddr;
   ipaddr       daddr;
} __attribute__ ((packed));

struct ICMPHeader
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
} __attribute__ ((packed));

struct UDPHeader
{
    uint16_t sourceport;
    uint16_t destport;
    uint16_t length;
    uint16_t checksum;
} __attribute__ ((packed));

struct TCPHeader
{
    uint16_t sourceport;
    uint16_t destport;
    uint32_t seqnumber;
    uint32_t acknumber;
    uint16_t flags;
    uint16_t window;
    uint32_t misc[2];
} __attribute__ ((packed));


uint8_t dummyMACCache[6];

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
        case ETHERNETII_TYPE_ARP:
              ProcessARPPacket(frame + sizeof(EthernetIIHeader), size - sizeof(EthernetIIHeader));

              break;

        case ETHERNETII_TYPE_IP:
              ProcessIPPacket(frame + sizeof(EthernetIIHeader), size - sizeof(EthernetIIHeader));

              break;

        case ETHERNETII_TYPE_IPV6:
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

    if ((arp->opcode == htons(ARP_OPCODE_REQUEST)) && (arp->targetIP == iface->myIP.addr)){
        DEBUG_MSG("ARP request\n");
        SendARPReply(arp);

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
                SendICMPReply(packet + sizeof(ICMPHeader), ipHeader->saddr);
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
    uint8_t newPacket[sizeof(EthernetIIHeader) + sizeof(ARPPacket)];

    BuildEthernetIIHeader(newPacket, arpPacket->senderMAC, ETHERNETII_TYPE_ARP);

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

    iface->send(newPacket, sizeof(EthernetIIHeader) + sizeof(ARPPacket));
}

void Net::SendICMPReply(uint8_t *data, ipaddr destIp)
{
    uint8_t newPacket[sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader) + 56]; //HARDCODED

    BuildEthernetIIHeader(newPacket, dummyMACCache, ETHERNETII_TYPE_IP);
    BuildIPHeader(newPacket + sizeof(EthernetIIHeader), destIp, 0x01, 0x54);

    ICMPHeader *newICMPHeader = (ICMPHeader *) (newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader));
    newICMPHeader->type = 0;
    newICMPHeader->code = 0;
    newICMPHeader->checksum = 0;
    memcpy(newPacket + sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader), data, 60);
    newICMPHeader->checksum = checksum((uint16_t *) newICMPHeader, 64);

    iface->send((const uint8_t *) newPacket, sizeof(EthernetIIHeader) + sizeof(IPHeader) + sizeof(ICMPHeader) + 60);
}

void Net::PrintIPAddr(uint32_t addr)
{
    ipaddr address;
    address.addr = addr;

    DEBUG_MSG("%d.%d.%d.%d", address.addrbytes[0], address.addrbytes[1], address.addrbytes[2], address.addrbytes[3]);
}
