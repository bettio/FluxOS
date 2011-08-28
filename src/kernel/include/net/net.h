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
 *   Name: net.h                                                           *
 ***************************************************************************/

#ifndef _NET_H_
#define _NET_H_

#include <net/ip.h>

#include <stdint.h>

struct ARPPacket;

struct NetIface
{
    uint8_t myMAC[6];
    ipaddr myIP;
    void (*send)(NetIface *iface, const uint8_t *packet, unsigned int size);
    void *card;
};

class Net
{
    public:
        void setIface(NetIface *i);
        void ProcessEthernetIIFrame(uint8_t *frame, int size);
        void ProcessARPPacket(uint8_t *packet, int size);
        void ProcessIPPacket(uint8_t *packet, int size);
        void ProcessICMPPacket(uint8_t *packet, int size);
        void ProcessUDPPacket(uint8_t *packet, int size);
        void ProcessTCPPacket(uint8_t *packet, int size);
        void BuildEthernetIIHeader(uint8_t *buffer, const uint8_t *destinationMAC, uint16_t type);
        void BuildIPHeader(uint8_t *buffer, ipaddr destinationIP, uint8_t protocol, uint16_t dataLen);
        void SendARPReply(const ARPPacket *arpPacket);
        void SendICMPReply(uint8_t *data, int size, ipaddr destIp);
        
    private:
        NetIface *iface;        
        uint8_t dummyMACCache[6];
};

#endif

