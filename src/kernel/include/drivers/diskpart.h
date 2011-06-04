/***************************************************************************
 *   Copyright 2007 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: diskpart.h                                                      *
 *   Date: 24/08/2007                                                      *
 ***************************************************************************/

#ifndef _DRIVERS_DISKPART_H_
#define _DRIVERS_DISKPART_H_

#include <drivers/blockdevicemanager.h>

#include <QHash>
#include <QList>
#include <QString>

#include <kdef.h>

struct Partition
{
	uint32_t FirstSector;
	uint32_t Length;
	BlockDevice *ParentBlockDevice;
};

class DiskPart
{
	public:
		static void Init();
		static bool CreatePartitionsDevices(BlockDevice *blkdev, const char *namingScheme);
		static bool CreateMSDOSPartitionsDevices(BlockDevice *blkdev, const char *namingScheme);
		static bool CreateGPTPartitionsDevices(BlockDevice *blkdev, const char *namingScheme);
		static void ReadBlock(BlockDevice *bd, int block, int blockn, uint8_t *blockbuffer);
		static bool WriteBlock(int block, int blockn, uint8_t *blockbuffer);
	private:
		static QList<char *> *BlockCache;
		static void RegisterPartition(BlockDevice *parent, const char *namingScheme, int partitionNum, uint32_t partitionStart, uint32_t partitionLength);
		static QHash<QString, Partition *> *Partitions;
};

#endif
