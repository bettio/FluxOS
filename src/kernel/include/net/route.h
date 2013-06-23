/***************************************************************************
 *   Copyright 2013 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: route.h                                                         *
 *   Date: 25/06/2013                                                      *
 ***************************************************************************/

#ifndef _ROUTE_H_
#define _ROUTE_H_

#include <net/nettypes.h>

//rtentry: it must be the same as the Linux one for compatibility reasons
struct rtentry {
    unsigned long       rt_pad1;
    struct sockaddr     rt_dst;
    struct sockaddr     rt_gateway;
    struct sockaddr     rt_genmask;
    unsigned short      rt_flags;
    short               rt_pad2;
    unsigned long       rt_pad3;
    void                *rt_pad4;
    short               rt_metric;
    char                *rt_dev;
    unsigned long       rt_mtu;
    unsigned long       rt_window;
    unsigned short      rt_irtt;
};

#endif

