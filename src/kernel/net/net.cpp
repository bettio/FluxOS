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
 *   Name: net.cpp                                                         *
 ***************************************************************************/

#include <net/net.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/tcp.h>

#include <core/printk.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

QList<NetIface *> *Net::interfaces;

void Net::init()
{
    interfaces = new QList<NetIface *>;

    IP::init();
    IPv6::init();
    UDP::init();
    TCP::init();
}

void Net::registerInterface(NetIface *iface)
{
    int id = interfaces->append(iface);

    ipaddr ip;
    ip.addrbytes[0] = 192;
    ip.addrbytes[1] = 168;
    ip.addrbytes[2] = 1;
    ip.addrbytes[3] = 5 + id;

    IP::addAddressToInterface(iface, ip);

    ipaddr dest;
    dest.addrbytes[0] = 192;
    dest.addrbytes[1] = 168;
    dest.addrbytes[2] = 1;
    dest.addrbytes[3] = 0;

    ipaddr mask;
    mask.addrbytes[0] = 255;
    mask.addrbytes[1] = 255;
    mask.addrbytes[2] = 255;
    mask.addrbytes[3] = 0;

    ipaddr gateway;
    gateway.addrbytes[0] = 0;
    gateway.addrbytes[1] = 0;
    gateway.addrbytes[2] = 0;
    gateway.addrbytes[3] = 0;

    IP::addRoute(dest, mask, gateway, iface);

    memset(iface->myIP6.addrbytes, 0, sizeof(IPv6Addr));
    iface->myIP6.addrbytes[0] = 0x20;
    iface->myIP6.addrbytes[1] = 0x01;
    iface->myIP6.addrbytes[2] = 0x04;
    iface->myIP6.addrbytes[3] = 0x70;
    iface->myIP6.addrbytes[4] = 0x00;
    iface->myIP6.addrbytes[5] = 0x6C;
    iface->myIP6.addrbytes[6] = 0x00;
    iface->myIP6.addrbytes[7] = 0x7E;
    iface->myIP6.addrbytes[15] = 0x3 + id;
}

