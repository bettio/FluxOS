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
}

