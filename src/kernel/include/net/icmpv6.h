/***************************************************************************
 *   Copyright 2012 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: icmpv6.h                                                        *
 *   Date: 19/02/2012                                                      *
 ***************************************************************************/

#ifndef _ICMP_H_
#define _ICMP_H_

#include <stdint.h>
#include <net/mac.h>
#include <net/ipv6.h>

#define ICMPV6_ECHO_REQUEST 128
#define ICMPV6_ECHO_REPLY 129
#define ICMPV6_NDP_NEIGHBOR_SOLICITATION 135
#define ICMPV6_NDP_NEIGHBOR_ADVERTISEMENT 136

struct ICMPv6Header
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
} __attribute__ ((packed));

struct NDPNeighborSolicitation
{
    uint32_t reserved;
    IPv6Addr targetAddress;
} __attribute__ ((packed));

struct NDPNeighborAdvertisement
{
    uint32_t flags;
    IPv6Addr targetAddress;
} __attribute__ ((packed));

struct NDPLinkLayerAddressOption
{
    uint8_t type;
    uint8_t len;
    macaddr linkLayerAddress;
} __attribute__ ((packed));

struct ICMPv6Echo
{
    uint16_t id;
    uint16_t seqN;
} __attribute__ ((packed));

struct NetIface;

class ICMPv6
{
    public:
        static void processICMPv6Packet(NetIface *iface, uint8_t *packet, int size, void *previousHeader, int previousHeaderType);
};

#endif
