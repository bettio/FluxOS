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
 *   Name: vt.h                                                            *
 *   Date: 07/10/2007                                                      *
 ***************************************************************************/

#ifndef _VT_H_
#define _VT_H_

#include <drivers/chardevicemanager.h>
#include <drivers/consolecolors.h>
#include <drivers/consoledevice.h>
#include <drivers/fbdevice.h>


class Vt
{
    public:
        static void init(ConsoleDevice *dev);
        static void notifyKeyPress(int code);
        static CharDevice *Device();
        static int Write(CharDevice *cd, const char *buffer, int count);
        static int Read(CharDevice *cd, char *buffer, int count);

        static int write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize);
        static int read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
        static int ioctl(VNode *node, int request, long arg);
        static void *mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset);

        static void ReInit();
    private:
        static ConsoleDevice *console;
        static CharDevice ttyDev;
        
        static void putc(char c);
};

#endif
