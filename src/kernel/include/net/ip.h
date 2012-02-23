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
 *   Name: ip.h                                                            *
 *   Date: 30/07/2011                                                      *
 ***************************************************************************/

#ifndef _IP_H_
#define _IP_H_

#include <stdint.h>
#include <QList>
#include <net/mac.h>

#define PROTOCOL_ICMP 1
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 17

#define IP_FLAGS_DF 0x4000

union ipaddr{
    uint32_t addr;
    uint8_t addrbytes[4];
}  __attribute__ ((packed));

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

struct IPFakeHeader
{
    ipaddr saddr;
    ipaddr daddr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t payloadLen;
};

struct NetIface;

struct Route
{
    ipaddr dest;
    ipaddr mask;
    ipaddr gateway;
    NetIface *iface;
};

class IP
{
    public:
        static void init();
        static void processIPPacket(NetIface *iface, uint8_t *packet, int size);
        static void processIPFragment(NetIface *iface, uint8_t *packet, int size);
        static uint16_t upperLayerChecksum(ipaddr saddr, ipaddr daddr, int protocol, void *header, int size);
        static void buildIPHeader(NetIface *iface, uint8_t *buffer, ipaddr destinationIP, uint8_t protocol, uint16_t dataLen);
        static void *allocPacketFor(NetIface *iface, void *buf, int size, ipaddr destIP, int protocol, int *offset);
        static void *allocPacketFor(void *buf, int size, ipaddr destIP, int protocol, int *offset);
        static void sendTo(NetIface *iface, void *buf, int size, ipaddr destIP, int protocol);
        static void sendTo(void *buf, int size, ipaddr destIP, int protocol);      
        static Route *route(ipaddr destIP);
        static bool route(ipaddr destIP, NetIface **destIf, macaddr *destMac);
        static void addRoute(ipaddr dest, ipaddr mask, ipaddr gateway, NetIface *iface);
        static void addAddressToInterface(NetIface *iface, ipaddr addr);
        static void forwardPacket(uint8_t *packet);


    private:
        static QList<Route *> *routes;
        static Route *defaultRoute;
        static bool ipForwardingEnabled;
};

#endif

