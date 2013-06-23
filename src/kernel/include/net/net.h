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
#include <net/netiface.h>

#include <QList>
#include <QString>

#include <stdint.h>

struct ARPPacket;

class Net
{
    public:
        static void init();
        static void registerInterface(NetIface *iface);
        static NetIface *interface(const char *);

    private:
        static QList<NetIface *> *interfaces;
        static QHash<QString, NetIface *> *interfacesByName;
};

#endif

