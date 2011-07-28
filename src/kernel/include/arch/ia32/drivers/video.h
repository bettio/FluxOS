/***************************************************************************
 *   Copyright 2004 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: video.h                                                         *
 *   Date: 16/09/2004                                                      *
 ***************************************************************************/

#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <drivers/chardevicemanager.h>
#include <drivers/consolecolors.h>
#include <drivers/consoledevice.h>

class Video{
    public:
        static void init();
        static void print(const char c);
        static void gotoXY(int x, int y);
        static void setBackColor(Color bkColor);
        static Color backColor();
        static void setTextColor(Color txtColor);
        static Color textColor();
        static int width();
        static int height();
        static int x();
        static void setX(int x);
        static int y();
        static void setY(int y);
        static void clear();

    private:
        static unsigned short *videomem;
        static unsigned char CColor;
        static int Cy;
        static int Cx;
        static ConsoleDevice consoleDevice;

        static void NewLine();
};

#endif
