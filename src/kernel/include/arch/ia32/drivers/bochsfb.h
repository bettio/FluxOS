/***************************************************************************
 *   Copyright 2016 by Davide Bettio <davide@uninstall.it>                 *
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
 *   Name: bochsfb.h                                                       *
 *   Date: 24/12/2016                                                      *
 ***************************************************************************/

#ifndef _BOCHSFB_H_
#define _BOCHSFB_H_

#include <stdint.h>
#include <drivers/fbdevice.h>
#include <filesystem/fstypes.h>
#include <filesystem/writeopflags.h>

struct VNode;
struct VBEModeInfoBlock;

class BochsFB
{
    public:
        static bool init(int bus, int slot);
        static bool registerDevice();

        static int write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize, WriteOpFlags flags);
        static int read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
        static int ioctl(VNode *node, int request, long arg);
        static void *mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset);

    private:
        static void setVideoMode(unsigned int width, unsigned int height, unsigned int bpp, int useLinearFrameBuffer, int clearVideoMemory);
        static void writeRegister(uint16_t indexValue, uint16_t dataValue);
        static uint16_t readRegister(uint16_t indexValue);
        static FBDevice fbDev;
};

#endif
