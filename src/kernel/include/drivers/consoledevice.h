/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: consoledevice.h                                                 *
 *   Date: 18/10/2010                                                      *
 ***************************************************************************/

#ifndef _CONSOLEDEVICE_H_
#define _CONSOLEDEVICE_H_

#include <stdint.h>
#include <drivers/consolecolors.h>

#define CONSOLEDEVICE(name, classname) \
        ConsoleDevice name = \
        { \
            classname::clear, \
            classname::print, \
            classname::backColor, \
            classname::setBackColor, \
            classname::textColor, \
            classname::setTextColor, \
            classname::height, \
            classname::width, \
            classname::gotoXY, \
            classname::x, \
            classname::setX, \
            classname::y, \
            classname::setY \
        };

struct ConsoleDevice
{
    void (*clear)();
    void (*print)(char c);
    Color (*backColor)();
    void (*setBackColor)(Color backColor);
    Color (*textColor)();
    void (*setTextColor)(Color txtColor);
    int (*height)();
    int (*width)();    
    void (*gotoXY)(int x, int y);
    int (*x)();
    void (*setX)(int x);
    int (*y)();
    void (*setY)(int y);
};

#endif
