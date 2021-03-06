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
QHash<QString, NetIface *> *Net::interfacesByName;

void Net::init()
{
    interfaces = new QList<NetIface *>;
    interfacesByName = new QHash<QString, NetIface *>;

    IP::init();
    IPv6::init();
    UDP::init();
    TCP::init();
}

void Net::registerInterface(NetIface *iface)
{
    int id = interfaces->append(iface);

    interfacesByName->insert("eth0", iface);

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

NetIface *Net::interface(const char *name)
{
    return interfacesByName->value(name);
}
