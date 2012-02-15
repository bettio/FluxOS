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
#include <net/mac.h>

#include <stdint.h>
#include <QHash>

inline int qHash(uint32_t v)
{
    return (int) v;
} 

struct NetIface
{
    macaddr myMAC;
    QHash<uint32_t, macaddr> macCache;
    ipaddr myIP;
    void *(*allocPacketFor)(NetIface *iface, void *buf, int size, macaddr destMAC, int protocol, int *offset);
    void (*sendTo)(NetIface *iface, void *buf, int size, macaddr destMAC, int protocol);
    void (*send)(NetIface *iface, const uint8_t *packet, unsigned int size);
    void *card;
};

#endif
