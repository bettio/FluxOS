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
 *   Name: timer.h                                                         *
 *   Date: 09/12/2011                                                      *
 ***************************************************************************/

#ifndef _ATA_H_
#define _ATA_H_

#include <stdint.h>

struct BlockDevice;

class ATA
{
    public:
        static void init();
        static void identifyDisk(const char *name, uint16_t baseAddr, uint16_t c);
        static BlockDevice *registerDisk(const char *name, int cookie);
        static void readBlock(BlockDevice *bd, int block, int blockn, uint8_t *blockbuffer);
};

#endif
