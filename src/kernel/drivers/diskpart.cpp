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
 *   Name: diskpart.cpp                                                    *
 *   Date: 24/08/2007                                                      *
 ***************************************************************************/

#include <drivers/diskpart.h>

#include <core/printk.h>

#include <cstring.h>
#include <cstdlib.h>

#include <kdef.h>

#define EXTENDED_PARTITION_TYPE 5
#define BR_SIGNATURE 0xAA55

QHash<QString, Partition *> *DiskPart::Partitions;

struct PartitionDescriptor
{
	uint8_t Status;
	struct{
		uint8_t Cylinder;
		uint8_t Head;
		uint8_t Sector;
	} FirstSectorCHS;
	uint8_t Type;
	struct{
		uint8_t Cylinder;
		uint8_t Head;
		uint8_t Sector;
	} LastSectorCHS;
	uint32_t FirstSectorLBA;
	uint32_t Length;
} __attribute__ ((packed));

struct MBR
{
	uint8_t Code[440];
	uint32_t DiskSignature;
	uint16_t Unused;
	PartitionDescriptor PrimaryPartitions[4];
	uint16_t Signature;
} __attribute__ ((packed));


struct EBR
{
	uint8_t Unused0[446];
	PartitionDescriptor LogicalPartitions[2];
	uint8_t Unused1[32];
	uint16_t Signature;
} __attribute__ ((packed));

struct GPTHeader
{
	char Signature[8];
	uint32_t Revision;
	uint32_t HeaderSize;
	uint32_t CRC32;
	uint32_t Reserved0;
	uint64_t PrimaryLBA;
	uint64_t BackupLBA;
	uint64_t FirstUsableLBA;
	uint64_t LastUsableLBA;
	uint8_t DiskGUID[16];
	uint64_t PartitionsLBA;
	uint32_t PartitionsNumber;
	uint32_t PartitionEntrySize;
	uint32_t PartitionsCRC32;	
	uint8_t Reserved[420];
};

struct GPTEntry
{
	uint8_t PartitionType[16];
	uint8_t PartitionGUID[16];
	uint64_t FirstLBA;
	uint64_t LastLBA;
	uint64_t Attributes;
	uint16_t PartitionName[36];
};

void DiskPart::Init()
{
	Partitions = new QHash<QString, Partition *>();
}

bool DiskPart::CreatePartitionsDevices(BlockDevice *blkdev, const char *namingScheme)
{
	if (CreateGPTPartitionsDevices(blkdev, namingScheme)) return true;

	if (CreateMSDOSPartitionsDevices(blkdev, namingScheme)) return true;

	return false;
}

bool DiskPart::CreateGPTPartitionsDevices(BlockDevice *blkdev, const char *namingScheme)
{
	GPTHeader gpt;

	blkdev->ReadBlock(blkdev, 1, 1, (uint8_t *) &gpt);

	if (memcmp(gpt.Signature, "EFI PART", 8) || (gpt.PartitionEntrySize != 128)){
		return false;
	}

	for (unsigned int lbaOffset = 0; lbaOffset < gpt.PartitionsNumber / 4; lbaOffset++){
		GPTEntry entries[4];
		blkdev->ReadBlock(blkdev, gpt.PartitionsLBA + lbaOffset, 1, (uint8_t *) entries);
		for (int i = 0; i < 4; i++){
			if (entries[i].FirstLBA != 0){
				RegisterPartition(blkdev, namingScheme, lbaOffset * 4 + i + 1,
						  (uint32_t) entries[i].FirstLBA, (uint32_t) (entries[i].LastLBA - entries[i].FirstLBA));
			}
		}
	}

	return true;
}

bool DiskPart::CreateMSDOSPartitionsDevices(BlockDevice *blkdev, const char *namingScheme)
{
	MBR mbr;

	blkdev->ReadBlock(blkdev, 0, 1, (uint8_t *) &mbr);

	if (mbr.Signature != BR_SIGNATURE){
		return false;
	}

	int j = 5;
	for (int i = 0; i < 4; i++){
		if (mbr.PrimaryPartitions[i].Length != 0)
			RegisterPartition(blkdev, namingScheme, i + 1, mbr.PrimaryPartitions[i].FirstSectorLBA, mbr.PrimaryPartitions[i].Length);

		if (mbr.PrimaryPartitions[i].Type == EXTENDED_PARTITION_TYPE){
			uint32_t lbaOffset = mbr.PrimaryPartitions[i].FirstSectorLBA;

			EBR ebr;
			do{
				blkdev->ReadBlock(blkdev, lbaOffset, 1, (uint8_t *) &ebr);

				if (ebr.Signature != BR_SIGNATURE){
					printk("Error: Invalid extended partition boot record signature: %i\n", ebr.Signature);
					return false; //TODO: or true?
				}

				if (ebr.LogicalPartitions[0].Length != 0)
					RegisterPartition(blkdev, namingScheme, j, lbaOffset, ebr.LogicalPartitions[0].Length);

				if (ebr.LogicalPartitions[1].Length != 0){
					j++;
					lbaOffset += ebr.LogicalPartitions[1].FirstSectorLBA;
				}

			}while(ebr.LogicalPartitions[1].Length != 0);
		}
	}

	return true;
}


void DiskPart::RegisterPartition(BlockDevice *parent, const char *namingScheme, int partitionNum, uint32_t partitionStart, uint32_t partitionLength)
{
	char *tmpStr = strdup(namingScheme);
    //TODO: Warning: uncheked malloc
    
	//Replace this code with something better
	int i = 0;
	while(tmpStr[i] != 0){
		if (tmpStr[i] == '#'){
			tmpStr[i] = partitionNum + '0';
		}
		i++;
	}

	Partition *part = new Partition();
	part->FirstSector = partitionStart;
	part->Length = partitionLength;
	part->ParentBlockDevice = parent;
	Partitions->insert(tmpStr, part);

	BlockDevice *tmpBlkDev = new BlockDevice;
	//TODO: Warning: unchecked malloc
    tmpBlkDev->Major = 0;
    tmpBlkDev->Minor = 0;
	tmpBlkDev->int_cookie = 0;
	tmpBlkDev->name = tmpStr;
	tmpBlkDev->ReadBlock = ReadBlock;
	BlockDeviceManager::Register(tmpBlkDev);
}

void DiskPart::ReadBlock(BlockDevice *bd, int block, int blockn, uint8_t *blockbuffer)
{
	Partition *p = Partitions->value(bd->name);

	p->ParentBlockDevice->ReadBlock(p->ParentBlockDevice, p->FirstSector + block, blockn, blockbuffer);
}

bool DiskPart::WriteBlock(int block, int blockn, uint8_t *blockbuffer){
	return false;
}
