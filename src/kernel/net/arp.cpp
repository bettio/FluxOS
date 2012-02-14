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
 *   Name: arp.cpp                                                         *
 ***************************************************************************/

#include <net/net.h>

#include <net/netutils.h>
#include <net/arp.h>
#include <net/ethernet.h>

#include <cstdlib.h>

#define IP_ADDRESS_LENGTH 4
#define MAC_ADDRESS_LENGTH 6

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

#define netBuffMalloc malloc

void ARP::processARPPacket(NetIface *iface, uint8_t *packet, int size)
{
    ARPPacket *arp = (ARPPacket *) packet;

    if ((arp->hardwareSize != MAC_ADDRESS_LENGTH) || (arp->protocolSize != IP_ADDRESS_LENGTH)){
        DEBUG_MSG("Unsupported address size: hardware: %i, protocol: %i\n", arp->hardwareSize, arp->protocolSize); 
    }

    uint64_t macAddr = 0;
    memcpy(&macAddr, arp->senderMAC, 6);
    iface->macCache.insert(arp->senderIP, macAddr);
    
    if ((arp->opcode == htons(ARP_OPCODE_REQUEST))){
        if (arp->targetIP == iface->myIP.addr){
            DEBUG_MSG("ARP request\n");
            sendARPReply(iface, arp);
        }else{
            DEBUG_MSG("ARP request (to other)\n");
        }

    }else if (arp->opcode == htons(ARP_OPCODE_REPLY)){
        DEBUG_MSG("Received ARP reply\n");

    }else{
        DEBUG_MSG("Unknown ARP opcode: %i\n", arp->opcode);
    }
}


void ARP::sendARPReply(NetIface *iface, const ARPPacket *arpPacket)
{
    uint8_t *newPacket = (uint8_t *) netBuffMalloc(sizeof(EthernetIIHeader) + sizeof(ARPPacket));

    Ethernet::buildEthernetIIHeader(iface, newPacket, arpPacket->senderMAC, ETHERTYPE_ARP);

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

void ARP::sendARPRequest(NetIface *iface, ipaddr ip)
{
    uint8_t *newPacket = (uint8_t *) netBuffMalloc(sizeof(EthernetIIHeader) + sizeof(ARPPacket));

    uint8_t broadcastMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    Ethernet::buildEthernetIIHeader(iface, newPacket, broadcastMAC, ETHERTYPE_ARP);

    ARPPacket *newArpPacket = (ARPPacket *) (newPacket + sizeof(EthernetIIHeader));
    newArpPacket->hardwareType = htons(1);
    newArpPacket->protocolType = htons(0x0800);
    newArpPacket->hardwareSize = 6;
    newArpPacket->protocolSize = 4;
    newArpPacket->opcode = htons(ARP_OPCODE_REQUEST);
    memcpy(newArpPacket->senderMAC, iface->myMAC, 6);
    newArpPacket->senderIP = iface->myIP.addr;
    memcpy(newArpPacket->targetMAC, broadcastMAC, 6);
    newArpPacket->targetIP = ip.addr;

    iface->send(iface, newPacket, sizeof(EthernetIIHeader) + sizeof(ARPPacket));  
}

