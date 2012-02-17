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
 *   Name: icmp.h                                                          *
 *   Date: 30/07/2011                                                      *
 ***************************************************************************/

#ifndef _ICMP_H_
#define _ICMP_H_

#include <stdint.h>

#define ICMP_ECHO_REPLY 0
#define ICMP_UNREACHABLE 3
#define ICMP_REDIRECT 5
#define ICMP_ECHO_REQUEST 8
#define ICMP_TIME_EXCEEDED 11
#define ICMP_TIMESTAMP_REQUEST 13
#define ICMP_TIMESTAMP_REPLY 14

#define ICMP_UNREACHABLE_NET 0
#define ICMP_UNREACHABLE_HOST 1
#define ICMP_UNREACHABLE_PROTOCOL 2
#define ICMP_UNREACHABLE_PORT 3
#define ICMP_UNREACHABLE_FRAGNEEDED 4
#define ICMP_UNREACHABLE_SOURCEROUTE 5

#define ICMP_TIME_EXCEEDED_TTL 0
#define ICMP_TIME_EXCEEDED__REASSEMBLY 1

struct ICMPHeader
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
} __attribute__ ((packed));

struct NetIface;

class ICMP
{
    public:
        static void processICMPPacket(NetIface *iface, uint8_t *packet, int size);
        static void sendICMPReply(NetIface *iface, uint8_t *data, int size, ipaddr destIp, int type, int code);
};

#endif
