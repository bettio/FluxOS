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

#include <stdint.h>

struct TCPHeader
{
    uint16_t sourceport;
    uint16_t destport;
    uint32_t seqnumber;
    uint32_t acknumber;
    uint16_t flags;
    uint16_t window;
    uint32_t misc[2];
} __attribute__ ((packed));

struct NetIface;

class TCP
{
    public:
        static void processTCPPacket(NetIface *iface, uint8_t *packet, int size);
};


#endif
