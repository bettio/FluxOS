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
 *   Name: ipv6.h                                                          *
 *   Date: 01/09/2011                                                      *
 ***************************************************************************/

#ifndef _IPV6_H_
#define _IPV6_H_

#include <stdint.h>

#define PROTOCOL_ICMPV6 58

struct NetIface;

struct IPv6Addr
{
    uint8_t addrbytes[16];
};

struct IPv6Header
{
    uint32_t head; //Version : 4, traffic class : 8, flow label: 20 
    uint16_t payloadLen;
    uint8_t nextHeader;
    uint8_t hopLimit;
    IPv6Addr saddr;
    IPv6Addr daddr;
};

struct IPv6FakeHeader
{
    IPv6Addr saddr;
    IPv6Addr daddr;
    uint32_t len;
    uint16_t zeros0;
    uint8_t zeros1;
    uint8_t nextHeader;
};

class IPv6
{
    public:
        static void init();
        static void processIPv6Packet(NetIface *iface, uint8_t *packet, int size);
        static void buildIPv6Header(NetIface *iface, uint8_t *buffer, IPv6Addr *destinationIP, uint8_t protocol, uint16_t dataLen);
        static uint16_t upperLayerChecksum(IPv6Addr *saddr, IPv6Addr *daddr, int protocol, void *header, int size);
        static void *allocPacketFor(NetIface *iface, void *buf, int size, IPv6Addr *destIP, int protocol, int *offset);
        static void sendTo(NetIface *iface, void *buf, int size, IPv6Addr *destIP, int protocol);
 
};

#endif

