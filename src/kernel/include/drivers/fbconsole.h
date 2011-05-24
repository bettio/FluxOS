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
 *   Name: fbconsole.h                                                     *
 *   Date: 19/06/2005                                                      *
 ***************************************************************************/

#ifndef _HARDWARE_DRIVER_VBECONSOLE_H
#define _HARDWARE_DRIVER_VBECONSOLE_H

#include <kdef.h>
#include <drivers/chardevicemanager.h>
#include <drivers/consolecolors.h>
#include <drivers/fbdevice.h>


class FBConsole
{
    public:
        static void init(FBDevice *dev);
        static void clear();
        static void print(const char c);
        static void gotoXY(int x, int y);                
        static void setBackColor(Color bkColor);
        static Color backColor();                
        static void setTextColor(Color txtColor);
        static Color textColor();             
        static int width();
        static int height();
        static int x(); 
        static int y();
        static void setX(int x);        
        static void setY(int y);

    private:
        static FBDevice *device;
        static uint16_t *charMem;
        static const char colors[16*3];
        static int Cy;
        static int Cx;
        static unsigned char CColor;

        static void scroll();
        static void DrawChar(unsigned short vchar, int vcharx, int vchary);
        static void DrawCursor(unsigned short vchar, int vcharx, int vchary);
        static void RedrawScreen();
        static void NewLine();
};

#endif
