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
 *   Name: ata.cpp                                                         *
 *   Date: 09/12/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/drivers/ata.h>

#include <arch/ia32/io.h>
#include <core/printk.h>
#include <drivers/blockdevice.h>
#include <drivers/diskpart.h>
#include <cstdlib.h>
#include <cstring.h>

#define ATA_PRIMARY_BASE_PORT 0x1F0
#define ATA_SECONDARY_BASE_PORT 0x170
#define ATA_MASTER 0xA0
#define ATA_SLAVE 0xB0

#define DATA_OFF 0
#define FEATURES_OFF 1
#define SECT_COUNT_OFF 2
#define LBA_LOW_OFF 3
#define LBA_MID_OFF 4
#define LBA_HIGH_OFF 5
#define DEVICE_OFF 6
#define COMMAND_OFF 7

void ATA::init()
{
    printk("Searching for ATA disks...\n");


    identifyDisk("disk0", ATA_PRIMARY_BASE_PORT, ATA_MASTER);
    identifyDisk("disk1", ATA_PRIMARY_BASE_PORT, ATA_SLAVE);
    identifyDisk("disk2", ATA_SECONDARY_BASE_PORT, ATA_MASTER);
    identifyDisk("disk3", ATA_SECONDARY_BASE_PORT, ATA_SLAVE);

    printk("ATA: done.\n");
}

BlockDevice *ATA::registerDisk(const char *name, int cookie)
{
    BlockDevice *disk = new BlockDevice;
    //TODO: Warning: unchecked malloc
    disk->int_cookie = cookie;
    disk->name = strdup(name);
    disk->ReadBlock = readBlock;
    BlockDeviceManager::Register(disk);
    int nameLen = strlen(name);
    char *newName = (char *) malloc(nameLen + 3);
    strcpy(newName, name);
    strcat(newName, "p#");
    DiskPart::CreatePartitionsDevices(disk, newName);

    return disk;;
}

void ATA::identifyDisk(const char *name, uint16_t baseAddr, uint16_t c)
{
    int result;

    outportb(baseAddr + 6, c);
    outportb(baseAddr + 2, 0);
    outportb(baseAddr + 3, 0);
    outportb(baseAddr + 4, 0);
    outportb(baseAddr + 5, 0);
    outportb(baseAddr + 7, 0xEC);
    result = inportb(baseAddr + 7);

    if (result){
        while (inportb(baseAddr + 7) & 0x80);
        if (inportb(baseAddr + 4) || inportb(baseAddr + 5)){
            return; // NOT ATA
        }
        do{
            result = inportb(baseAddr + 7);
        }while (!((result & 8) || (result & 1)));
        if (!(result & 1)){
            uint16_t *buf = (uint16_t *) malloc(512);
            for (int i = 0; i < 256; i++){
                buf[i] = inport16(baseAddr + 0);
            }
            if (buf[83] & (1 << 10)){
                printk("Found ATA disk: /dev/%s, size: 0x%llx sectors\n", name, *((uint64_t *) &buf[100]));
                registerDisk(name, (baseAddr << 16) | c);
            }else if (*((uint32_t *) &buf[60])){
                printk("Found ATA disk: /dev/%s,  size: 0x%x sectors\n", name, *((uint32_t *) &buf[60]));
                registerDisk(name, (baseAddr << 16) | c);
            }
        }
    }
}

void ATA::readBlock(BlockDevice *bd, int block, int blockn, uint8_t *blockbuffer)
{
    int baseAddr = bd->int_cookie >> 16;
    int c = bd->int_cookie & 1;

    for (int b = block; b < block + blockn; b++){
        outportb(baseAddr + FEATURES_OFF, 0x00);
        outportb(baseAddr + SECT_COUNT_OFF, blockn);
        outportb(baseAddr + LBA_LOW_OFF, b & 0xFF);
        outportb(baseAddr + LBA_MID_OFF, ((b & 0xFF00) >> 8));
        outportb(baseAddr + LBA_HIGH_OFF, ((b & 0xFF0000) >> 16));
        outportb(baseAddr + DEVICE_OFF, 0xE0 | (c << 4) |  ((b & 0xF000000) >> 24));
        outportb(baseAddr + COMMAND_OFF, 0x20);

        while (!(inportb(baseAddr + DATA_OFF) & 0x08));

        unsigned int bytesToRead = blockn * 256;

        for (unsigned int i = 0; i < bytesToRead; i++){
            ((uint16_t *) (blockbuffer + (b - block)*512))[i] = inport16(baseAddr + DATA_OFF); 
        }
    }
}

