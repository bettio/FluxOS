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
 *   Name: vesafb.cpp                                                      *
 *   Date: 20/12/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/drivers/vesafb.h>

#include <drivers/fbconsole.h>
#include <drivers/fbdevice.h>
#include <drivers/fb24bppfunctions.h>
#include <drivers/fb32bppfunctions.h>
#include <drivers/genericfbdevice.h>
#include <drivers/chardevicemanager.h>
#include <drivers/chardevice.h>
#include <filesystem/vnode.h>
#include <errors.h>
#include <arch/ia32/mm/pagingmanager.h>

struct VBEModeInfoBlock
{
    unsigned short modeAttributes;
    uint8_t winAAtributes;
    uint8_t winBAttributes;
    unsigned short winGranularity;
    unsigned short winSize;
    unsigned short winASegment;
    unsigned short winBSegment;
    unsigned int winFuncPtr;
    unsigned short bytesPerScanline;
    unsigned short xRes;
    unsigned short yRes;
    uint8_t xCharSize;
    uint8_t yCharSize;
    uint8_t numberOfPlanes;
    uint8_t bpp;
    uint8_t numberOfBanks;
    uint8_t memoryModel;
    uint8_t bankSize;
    uint8_t numberOfImagePages;
    uint8_t reserved0;
    uint8_t redMaskSize;
    uint8_t redFieldPosition;
    uint8_t greenMaskSize;
    uint8_t greenFieldPosition;
    uint8_t blueMaskSize;
    uint8_t blueFieldPosition;
    uint8_t reservedMaskSize;
    uint8_t reservedFieldPosition;
    uint8_t directColorModeInfo;
    unsigned int physicalBasePtr;
    unsigned int offscreenMemoryOffset;
    unsigned short offscreenMemorySize;
} __attribute__((packed));

struct VBESVGAInfoBlock
{
    uint8_t signature[4];
    unsigned short version;
    char *oemString;
    uint8_t capabilities[4];
    unsigned short *videoMode;
    unsigned short totalMemory;
    unsigned short oemSoftwareRev;
    char *oemVendorName;
    char *oemProductName;
    char *oemProductRev;
} __attribute__((packed));

VBEModeInfoBlock *VesaFB::vbeInfo;
uint8_t *VesaFB::videoMem;
FBDevice VesaFB::fbDev;

bool VesaFB::init(unsigned long modeInfoBlockAddr)
{
    vbeInfo = (VBEModeInfoBlock *) modeInfoBlockAddr;
    videoMem = (uint8_t *) vbeInfo->physicalBasePtr;

    fbDev.bpp = vbeInfo->bpp;
    if (vbeInfo->bpp == 32){
        fbDev.clear = Fb32bppFunctions::clear;
        fbDev.drawMonoPixmap = Fb32bppFunctions::drawMonoPixmap;
        fbDev.fillRectange = Fb32bppFunctions::fillRectange;

    }else if(vbeInfo->bpp == 24){
        fbDev.clear = Fb24bppFunctions::clear;
        fbDev.drawMonoPixmap = Fb24bppFunctions::drawMonoPixmap;
        fbDev.fillRectange = Fb24bppFunctions::fillRectange;

    }else{
        videoMem = 0;
        return false;
    }
    fbDev.width = vbeInfo->xRes;
    fbDev.height = vbeInfo->yRes;
    fbDev.framebuffer = videoMem;

    FBConsole::init(&fbDev);

    return true;
}

bool VesaFB::mapPhysicalMem()
{
    if (videoMem){
        videoMem = (uint8_t *) PagingManager::mapPhysicalMemory((uint32_t) videoMem, vbeInfo->xRes*vbeInfo->yRes*vbeInfo->bpp / 8);
        fbDev.framebuffer = videoMem;
        return true;

    }else{
        return false;
    }
}

#include <core/printk.h>

bool VesaFB::registerDevice()
{
    if (!videoMem){
        return false;
    }

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

int VesaFB::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize, WriteOpFlags flags)
{
    return 0;
}

int VesaFB::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    return 0;    
}

int VesaFB::ioctl(VNode *node, int request, long arg)
{
    return GenericFBDevice::ioctl(&fbDev, node, request, arg);
}

void *VesaFB::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return GenericFBDevice::mmap(&fbDev, node, start, length, prot, flags, fd, offset);
}

