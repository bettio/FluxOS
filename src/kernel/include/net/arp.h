/***************************************************************************
 *   Copyright 2011 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: arp.h                                                          *
 *   Date: 30/07/2011                                                      *
 ***************************************************************************/

#ifndef _ARP_H_
#define _ARP_H_

#include <stdint.h>

#define ARP_OPCODE_REQUEST 0x0001
#define ARP_OPCODE_REPLY 0x0002

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

struct NetIface;

class ARP
{
    public:
        static void processARPPacket(NetIface *iface, uint8_t *packet, int size);
        static void sendARPReply(NetIface *iface, const ARPPacket *arpPacket);
};

#endif
