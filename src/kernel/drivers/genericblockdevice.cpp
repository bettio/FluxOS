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
 *   Name: genericblockdevice.cpp                                          *
 *   Date: 16/12/2011                                                      *
 ***************************************************************************/

#include <drivers/genericblockdevice.h>

#include <drivers/blockdevice.h>

int GenericBlockDevice::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    BlockDevice *blkDev = (BlockDevice *) node->privdata;
    void (*readBlock) (BlockDevice *bd, int block, int blocksN, uint8_t *blockbuffer) = blkDev->ReadBlock;


    uint64_t readBytes = 0;

    if (pos % 512 != 0){
        char tmpBuf[512];
        readBlock(blkDev, pos / 512, 1, (uint8_t *) tmpBuf);
	readBytes = MIN(bufsize, 512 - (pos % 512));
        memcpy(buffer, tmpBuf + (pos % 512), readBytes);
    }

    if (bufsize > 512 || bufsize % 512 == 0){
        readBlock(blkDev, (pos + readBytes) / 512, (bufsize - readBytes) / 512, (uint8_t *) buffer + readBytes);
	readBytes += ((bufsize - readBytes) / 512) * 512;
    }

    if (readBytes < bufsize){
        char tmpBuf[512];
        readBlock(blkDev, (pos + readBytes) / 512, 1, (uint8_t *) tmpBuf);
        memcpy(buffer + readBytes, tmpBuf, bufsize - readBytes);
    }

    return bufsize;
}
