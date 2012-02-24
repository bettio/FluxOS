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
 *   Name: netutils.cpp                                                    *
 ***************************************************************************/

#include <net/net.h>

#include <net/netutils.h>

#include <cstdlib.h>
#include <stdint.h>

uint16_t checksum(uint16_t *data, int size)
{
    uint32_t sum = 0;

    int i;
    for (i = 0; i < size / 2; i++){
        sum += ntohs(data[i]);
    }
    if (size % 2){
        uint16_t oddTmp = data[i];
        ((uint8_t *) &oddTmp)[1] = 0;
        sum += ntohs(oddTmp);
    }

    return htons(~((sum & 0xFFFF) + (sum >> 16)));
}

uint16_t checksum(uint16_t *fakeHeader, int fakeSize, uint16_t *data, int size){
    uint32_t sum = 0;

    for (int i = 0; i < fakeSize / 2; i++){
        sum += ntohs(fakeHeader[i]);
    }

    int i;
    for (i = 0; i < size / 2; i++){
        sum += ntohs(data[i]);
    }
    if (size % 2){
        uint16_t oddTmp = data[i];
        ((uint8_t *) &oddTmp)[1] = 0;
        sum += ntohs(oddTmp);
    }

    return htons(~((sum & 0xFFFF) + (sum >> 16)));
}

