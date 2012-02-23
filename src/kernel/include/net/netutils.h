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
 *   Name: netutils.h                                                      *
 *   Date: 28/08/2011                                                      *
 ***************************************************************************/

#ifndef _NETUTILS_H_
#define _NETUTILS_H_

#include <endian.h>

inline uint32_t htonl(uint32_t hostlong)
{
    return HOST_TO_BIG_32(hostlong);
}

inline int16_t htons(uint16_t hostshort)
{
    return HOST_TO_BIG_16(hostshort);
}

inline uint32_t ntohl(uint32_t netlong)
{
    return BIG_TO_HOST_32(netlong);
}

inline uint16_t ntohs(uint16_t netshort)
{
    return BIG_TO_HOST_16(netshort);
}

uint16_t checksum(uint16_t *data, int size);
uint16_t checksum(uint16_t *fakeHeader, int fakeSize, uint16_t *data, int size);

void ipv4toString(uint32_t addr, char *str);

#endif

