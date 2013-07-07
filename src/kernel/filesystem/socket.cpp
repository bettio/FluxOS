/***************************************************************************
 *   Copyright 2012 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: socket.cpp                                                      *
 ***************************************************************************/

#include <filesystem/socket.h>

#include <core/printk.h>

#include <filesystem/vfs.h>
#include <filesystem/vnodemanager.h>

#include <net/ipsocketcalls.h>
#include <net/ip6socketcalls.h>
#include <net/udpsocketcalls.h>
#include <net/nettypes.h>

using namespace FileSystem;

unsigned long long Socket::socketsCounter;

int Socket::init()
{
    return 0;
}

VNode *Socket::newSocket(int domain, int type, int protocol)
{
    VNode *vnd;
    VNodeManager::GetVnode(SOCKET_MOUNTID, socketsCounter, &vnd);
    if (vnd == NULL) return NULL;

    if ((domain == PF_INET) && (type == SOCK_DGRAM) && (protocol == 0)){
        UDPSocketCalls::bindToSocket(vnd, domain, type, protocol);

    }else if  ((domain == PF_INET6) && (type == SOCK_DGRAM) && (protocol == 0)){
        UDPSocketCalls::bindToSocket(vnd, domain, type, protocol);
    }

    socketsCounter++;
    
    return vnd;
}

