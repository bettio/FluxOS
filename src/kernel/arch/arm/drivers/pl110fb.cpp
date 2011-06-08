/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: pl110fb.cpp                                                     *
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/

#include <arch/arm/drivers/pl110fb.h>
#include <drivers/fbconsole.h>
#include <drivers/fb32bppfunctions.h>

#include <cstdlib.h>
#include <stdint.h>

#define LCDTIM0 0x00
#define LCDTIM1 0x04 
#define LCDUPBASE 0x10 
#define LCDCONTROL 0x18

void *Pl110Fb::fb;

void Pl110Fb::init()
{
    fb = malloc(1024*768*5);
    *((volatile uint32_t *) (0x10120000 + LCDTIM0)) = 0x3F << 2;
    *((volatile uint32_t *) (0x10120000 + LCDTIM1)) = 767;
    *((volatile uint32_t *) (0x10120000 + LCDUPBASE)) =  (uint32_t) fb;
    *((volatile uint32_t *) (0x10120000 + LCDCONTROL)) = 0xB | 1 << 5 | 1 << 11;

    FBDevice *fbDev = new FBDevice;
    fbDev->width = 1024;
    fbDev->height = 768;
    fbDev->bpp = 32;
    fbDev->framebuffer = fb;
    fbDev->clear = Fb32bppFunctions::clear;
    fbDev->drawMonoPixmap = Fb32bppFunctions::drawMonoPixmap;
    fbDev->fillRectange = Fb32bppFunctions::fillRectange;
    
    FBConsole::init(fbDev);
}
