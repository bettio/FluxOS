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
 *   Name: ethernet.h                                                      *
 *   Date: 30/07/2011                                                      *
 ***************************************************************************/

#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include <net/mac.h>
#include <stdint.h>

#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPV6 0x86DD

struct EthernetIIHeader
{
  macaddr destination;
  macaddr source;
  uint16_t type;
} __attribute__ ((packed));;

struct NetIface;

class Ethernet
{
    public:
        static void *allocPacketFor(NetIface *iface, void *buf, int size, macaddr destMAC, int protocol, int *offset);
        static void sendTo(NetIface *iface, void *buf, int size, macaddr destMAC, int protocol);
        static void processEthernetIIFrame(NetIface *iface, uint8_t *frame, int size);
        static void buildEthernetIIHeader(NetIface *iface, uint8_t *buffer, macaddr destinationMAC, uint16_t type);
};

#endif
