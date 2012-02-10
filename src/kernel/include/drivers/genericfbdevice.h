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
 *   Name: genericfbdevice.cpp                                             *
 *   Date: 11/01/2012                                                      *
 ***************************************************************************/

#ifndef _DRIVERS_GENERICFBDEVICE_H_
#define _DRIVERS_GENERICFBDEVICE_H_

#include <filesystem/fstypes.h>

struct FBDevice;
struct VNode;

class GenericFBDevice
{
	public:
            static int ioctl(FBDevice *dev, VNode *node, int request, long arg);
            static void *mmap(FBDevice *dev, VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset);
};

#endif

