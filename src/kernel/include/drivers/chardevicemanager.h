/***************************************************************************
 *   Copyright 2005 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: chardevicemanager.h                                             *
 *   Date: 27/12/2006                                                      *
 ***************************************************************************/

#ifndef _DRIVERS_CHARDEVICEMANAGER_H_
#define _DRIVERS_CHARDEVICEMANAGER_H_

#include <drivers/chardevice.h>

class CharDeviceManager
{
	public:
		static void Init();
		static int Register(CharDevice *cd);
		static int Unregister(const char *name);

		static CharDevice *Device(char *name);
		static CharDevice *Device(int major, int minor);
};

#endif
