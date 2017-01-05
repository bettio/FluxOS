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
 *   Name: bochsfb.cpp                                                     *
 *   Date: 24/12/2016                                                      *
 ***************************************************************************/

#include <arch/ia32/drivers/bochsfb.h>

#include <drivers/fb24bppfunctions.h>
#include <drivers/fb32bppfunctions.h>

#include <arch/ia32/core/pci.h>
#include <arch/ia32/io.h>
#include <core/printk.h>
#include <drivers/genericfbdevice.h>
#include <drivers/chardevicemanager.h>
#include <drivers/chardevice.h>

#define BOCHSFB_DISPI_IOPORT_INDEX           0x01CE
#define BOCHSFB_DISPI_IOPORT_DATA            0x01CF

#define BOCHSFB_DISPI_INDEX_ID               0x0
#define BOCHSFB_DISPI_INDEX_XRES             0x1
#define BOCHSFB_DISPI_INDEX_YRES             0x2
#define BOCHSFB_DISPI_INDEX_BPP              0x3
#define BOCHSFB_DISPI_INDEX_ENABLE           0x4
#define BOCHSFB_DISPI_INDEX_BANK             0x5
#define BOCHSFB_DISPI_INDEX_VIRT_WIDTH       0x6
#define BOCHSFB_DISPI_INDEX_VIRT_HEIGHT      0x7
#define BOCHSFB_DISPI_INDEX_X_OFFSET         0x8
#define BOCHSFB_DISPI_INDEX_Y_OFFSET         0x9
#define BOCHSFB_DISPI_INDEX_VIDEO_MEMORY_64K 0xa

#define BOCHSFB_DISPI_ID0                    0xB0C0
#define BOCHSFB_DISPI_ID1                    0xB0C1
#define BOCHSFB_DISPI_ID2                    0xB0C2
#define BOCHSFB_DISPI_ID3                    0xB0C3
#define BOCHSFB_DISPI_ID4                    0xB0C4
#define BOCHSFB_DISPI_ID5                    0xB0C5

#define BOCHSFB_DISPI_DISABLED               0x00
#define BOCHSFB_DISPI_ENABLED                0x01
#define BOCHSFB_DISPI_GETCAPS                0x02
#define BOCHSFB_DISPI_8BIT_DAC               0x20
#define BOCHSFB_DISPI_LFB_ENABLED            0x40
#define BOCHSFB_DISPI_NOCLEARMEM             0x80

FBDevice BochsFB::fbDev;

bool BochsFB::init(int bus, int slot)
{
    uint32_t ioBase = PCI::read(bus, slot, 0, PCI_IOBASE) & ~1;

    printk("Found Bochs video PCI card on bus: %i slot: %i, iobase: 0x%x\n", bus, slot, ioBase);

    fbDev.bpp = 32;
    if (fbDev.bpp == 32){
        fbDev.clear = Fb32bppFunctions::clear;
        fbDev.drawMonoPixmap = Fb32bppFunctions::drawMonoPixmap;
        fbDev.fillRectange = Fb32bppFunctions::fillRectange;

    }else if(fbDev.bpp == 24){
        fbDev.clear = Fb24bppFunctions::clear;
        fbDev.drawMonoPixmap = Fb24bppFunctions::drawMonoPixmap;
        fbDev.fillRectange = Fb24bppFunctions::fillRectange;

    }else{
        return false;
    }

    fbDev.width = 1024;
    fbDev.height = 768;
    fbDev.fbMemLen = 1024*768*4;
    fbDev.framebuffer = (uint8_t *) ioBase;

    registerDevice();

    return true;
}

void BochsFB::writeRegister(uint16_t indexValue, uint16_t dataValue)
{
    outport16(BOCHSFB_DISPI_IOPORT_INDEX, indexValue);
    outport16(BOCHSFB_DISPI_IOPORT_DATA, dataValue);
}

uint16_t BochsFB::readRegister(uint16_t indexValue)
{
    outport16(BOCHSFB_DISPI_IOPORT_INDEX, indexValue);
    return inport16(BOCHSFB_DISPI_IOPORT_DATA);
}

void BochsFB::setVideoMode(unsigned int width, unsigned int height, unsigned int bpp, int useLinearFrameBuffer, int clearVideoMemory)
{
    writeRegister(BOCHSFB_DISPI_INDEX_ENABLE, BOCHSFB_DISPI_DISABLED);
    writeRegister(BOCHSFB_DISPI_INDEX_XRES, width);
    writeRegister(BOCHSFB_DISPI_INDEX_YRES, height);
    writeRegister(BOCHSFB_DISPI_INDEX_BPP, bpp);
    writeRegister(BOCHSFB_DISPI_INDEX_ENABLE, BOCHSFB_DISPI_ENABLED |
        (useLinearFrameBuffer ? BOCHSFB_DISPI_LFB_ENABLED : 0) |
        (clearVideoMemory ? 0 : BOCHSFB_DISPI_NOCLEARMEM));
}

int BochsFB::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize, WriteOpFlags flags)
{
    //TODO: check parameters
    memcpy((uint8_t *) fbDev.framebuffer + pos, buffer, bufsize);
    return bufsize;
}

int BochsFB::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    //TODO: check parameters
    memcpy(buffer, (uint8_t *) fbDev.framebuffer + pos, bufsize);
    return bufsize;
}

int BochsFB::ioctl(VNode *node, int request, long arg)
{
    return GenericFBDevice::ioctl(&fbDev, node, request, arg);
}

void *BochsFB::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    setVideoMode(1024, 768, 32, 1,  0);
    return GenericFBDevice::mmap(&fbDev, node, start, length, prot, flags, fd, offset);
}

bool BochsFB::registerDevice()
{
    CharDevice *fb0 = new CharDevice;
    if (fb0 == NULL){
        return false;
    }
    fb0->int_cookie = 0;
    fb0->name = "fb0";
    fb0->Major = 89;
    fb0->Minor = 0;
    fb0->read = read;
    fb0->write = write;
    fb0->ioctl = ioctl;
    fb0->mmap = mmap;
    CharDeviceManager::Register(fb0);

    return true;
}

