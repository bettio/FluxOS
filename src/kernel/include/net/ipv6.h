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
 *   Name: ipv6.h                                                          *
 *   Date: 01/09/2011                                                      *
 ***************************************************************************/

IPv6Addr
{
    uint8_t addr[8];
};

IPv6Header
{
    uint32_t head; //Version : 4, traffic class : 8, flow label: 20 
    uint16_t payloadLen;
    uint8_t nextHeader;
    uint8_t hopLimit;
    IPv6Addr src;
    IPv6Addr dest;
};

ICMPv6Packet
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
};
