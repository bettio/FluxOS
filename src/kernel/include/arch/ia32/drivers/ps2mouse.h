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
 *   Name: ps2mouse.h                                                      *
 *   Date: 18/12/2011                                                      *
 ***************************************************************************/

#ifndef _PS2MOUSE_H_
#define _PS2MOUSE_H_

#include <drivers/chardevice.h>

class PS2Mouse
{
    public:
        static void init();
        static void mouseInterruptHandler();
        static int read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
        static void waitController();
        static uint8_t readPort0x60();
        static void writePort0x60(uint8_t value);
        static void writePort0x64(uint8_t value);

    private:
        static CharDevice mouseDev;
};

#endif
