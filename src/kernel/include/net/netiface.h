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
 *   Name: netiface.h                                                      *
 *   Date: 28/08/2011                                                      *
 ***************************************************************************/

#ifndef _NETIFACE_H_
#define _NETIFACE_H_

#include <net/ip.h>
#include <net/ipv6.h>
#include <net/mac.h>

#include <stdint.h>
#include <QHash>

class IPv6AddrPtr
{
    public:
        IPv6Addr *addr;

        inline IPv6AddrPtr()
        {
            addr = 0;
        }

        inline IPv6AddrPtr(IPv6Addr *a)
        {
            addr = a;
        }

        inline bool operator==(const IPv6AddrPtr &other) const
        {
            bool equals = true;
            for (int i = 0; i < 16; i++){
                if (addr->addrbytes[i] != other.addr->addrbytes[i]){
                    equals = false;
                    break;
                }
            }
            return equals;
        }
};

inline int qHash(uint32_t v)
{
    return (int) v;
} 

inline int qHash(const IPv6AddrPtr &ptr)
{
    IPv6Addr *addr = ptr.addr;   
    int sum = 0;
    for (int i = 0; i < 16; i++){
        sum += addr->addrbytes[i];
    }
    return sum;
}

struct NetIface
{
    macaddr myMAC;
    QHash<uint32_t, macaddr> macCache;
    QHash<IPv6AddrPtr, macaddr> macCache6;
    ipaddr myIP;
    IPv6Addr myIP6;
    int mtu;
    void *(*allocPacketFor)(NetIface *iface, void *buf, int size, macaddr destMAC, int protocol, int *offset);
    void (*sendTo)(NetIface *iface, void *buf, int size, macaddr destMAC, int protocol);
    void (*send)(NetIface *iface, const uint8_t *packet, unsigned int size);
    void *card;
};

#endif
