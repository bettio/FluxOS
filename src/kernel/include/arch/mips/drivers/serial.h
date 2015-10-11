/***************************************************************************
 *   Copyright 2010,2015 by Davide Bettio <davide.bettio@kdemail.net>      *
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
 *   Name: serial.h                                                        *
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/

#ifndef _ARCH_MIPS_SERIAL_H_
#define _ARCH_MIPS_SERIAL_H_

#include <filesystem/vfs.h>

struct CharDevice;
struct VNode;

class Serial
{
    public:
        static void init();
        static void reinit();
        static CharDevice *Device();
        static int Write(CharDevice *cd, const char *buffer, int count);
        static int Read(CharDevice *cd, char *buffer, int count);
        static int write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize);
        static int read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
        static int ioctl(VNode *node, int request, long arg);
        static void *mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset);

    private:
        static CharDevice serialTty;
};

#endif
