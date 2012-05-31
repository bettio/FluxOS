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

void ARP::processARPPacket(NetIface *iface, uint8_t *packet, int size)
{
    ARPPacket *arp = (ARPPacket *) packet;

    if ((arp->hardwareSize != MAC_ADDRESS_LENGTH) || (arp->protocolSize != IP_ADDRESS_LENGTH)){
        DEBUG_MSG("ARP: unsupported address size: hardware: %i, protocol: %i\n", arp->hardwareSize, arp->protocolSize); 
    }

    iface->macCache.insert(arp->senderIP, arp->senderMAC);
    
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
    int payloadOffset;
    uint8_t *newPacket = (uint8_t *) iface->allocPacketFor(iface, 0, sizeof(ARPPacket), arpPacket->senderMAC, ETHERTYPE_ARP, &payloadOffset);

    ARPPacket *newArpPacket = (ARPPacket *) (newPacket + payloadOffset);
    newArpPacket->hardwareType = htons(1);
    newArpPacket->protocolType = htons(0x0800);
    newArpPacket->hardwareSize = 6;
    newArpPacket->protocolSize = 4;
    newArpPacket->opcode = htons(ARP_OPCODE_REPLY);
    newArpPacket->senderMAC = iface->myMAC;
    newArpPacket->senderIP = arpPacket->targetIP;
    newArpPacket->targetMAC = arpPacket->senderMAC;
    newArpPacket->targetIP = arpPacket->senderIP;

    iface->sendTo(iface, newPacket, sizeof(ARPPacket), arpPacket->senderMAC, ETHERTYPE_ARP);
}

void ARP::sendARPRequest(NetIface *iface, ipaddr ip)
{
    macaddr broadcastMAC;
    broadcastMAC.addrbytes[0] = 0xFF;
    broadcastMAC.addrbytes[1] = 0xFF;
    broadcastMAC.addrbytes[2] = 0xFF;
    broadcastMAC.addrbytes[3] = 0xFF;
    broadcastMAC.addrbytes[4] = 0xFF;
    broadcastMAC.addrbytes[5] = 0xFF;
   
    int payloadOffset;
    uint8_t *newPacket = (uint8_t *) iface->allocPacketFor(iface, 0, sizeof(ARPPacket), broadcastMAC, ETHERTYPE_ARP, &payloadOffset);

    ARPPacket *newArpPacket = (ARPPacket *) (newPacket + sizeof(EthernetIIHeader));
    newArpPacket->hardwareType = htons(1);
    newArpPacket->protocolType = htons(0x0800);
    newArpPacket->hardwareSize = 6;
    newArpPacket->protocolSize = 4;
    newArpPacket->opcode = htons(ARP_OPCODE_REQUEST);
    newArpPacket->senderMAC = iface->myMAC;
    newArpPacket->senderIP = iface->myIP.addr;
    newArpPacket->targetMAC = broadcastMAC;
    newArpPacket->targetIP = ip.addr;

    iface->sendTo(iface, newPacket, sizeof(ARPPacket), broadcastMAC, ETHERTYPE_ARP);
}

