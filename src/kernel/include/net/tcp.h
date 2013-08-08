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
 *   Name: tcp.h                                                           *
 *   Date:28/08/2011                                                       *
 ***************************************************************************/

#ifndef _TCP_H_
#define _TCP_H_

class VNode;
template <typename Key, typename T> class QHash;

#include <stdint.h>
#include <net/ip.h>

#define TCP_FLAGS_FIN 1
#define TCP_FLAGS_SYN 2
#define TCP_FLAGS_RST 4
#define TCP_FLAGS_PSH 8
#define TCP_FLAGS_ACK 16
#define TCP_FLAGS_URG 32
#define TCP_FLAGS_ECE 64
#define TCP_FLAGS_CWR 128

struct TCPHeader
{
    uint16_t sourceport;
    uint16_t destport;
    uint32_t seqnumber;
    uint32_t acknumber;
    uint16_t flags;
    uint16_t windowSize;
    uint16_t checksum;
    uint16_t urgentPtr;
} __attribute__ ((packed));

struct NetIface;

class TCP
{
    public:
        static void init();
        static void processTCPPacket(NetIface *iface, uint8_t *packet, int size, void *previousHeader, int previousHeaderType);
        static void sendTCPPacket(NetIface *iface, ipaddr srcIP, ipaddr destIp, uint16_t srcPort, uint16_t destPort, int flags, int acknumber, int seqnumber, uint8_t *packet, int size);
        static int bindVNodeToPort(uint16_t port, VNode *node);

    private:
        static QHash<uint16_t, VNode *> *openPorts;
};


#endif
