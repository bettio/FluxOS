/***************************************************************************
 *   Copyright 2005 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: physicalmm.h                                                    *
 *   Date: 23/09/2005                                                      *
 ***************************************************************************/

#include <arch/ia32/mm/physicalmm.h>

#include <core/printk.h>
#include <core/systemerrors.h>
#include <cstring.h>
#include <cstdlib.h>
#include <gccbuiltins.h>

#define BITMAP_SIZE 0x20000

uint32_t *pageBitmap;
uint32_t freePagesNum = BITMAP_SIZE; //USE A REAL COUNT

void PhysicalMM::init()
{
    //TODO: use a smaller bitmap, this bitmap covers all the possible 32 bit physical address space
    pageBitmap = (uint32_t *) malloc(BITMAP_SIZE);
    memset(pageBitmap, 0, BITMAP_SIZE);
}

void PhysicalMM::setAllocatedPage(uint32_t addr)
{
    pageBitmap[addr / 4096 / 32] |= (1 << ((addr / 4096) % 32));
    freePagesNum--;
}

uint32_t PhysicalMM::allocPage()
{
    for (int i = 0; i < BITMAP_SIZE / 4 /*4 bytes for each item*/; i++){
        if (pageBitmap[i] != 0xFFFFFFFF){
            for (int j = 0; j < 32; j++){
                if (!(pageBitmap[i] & (1 << j))){
                    pageBitmap[i] |= (1 << j);
                    freePagesNum--;
                    return (i*32 + j)*4096;
                }
            }
        }
    }
    
    return (uint32_t) -1;
}

void PhysicalMM::freePage(uint32_t addr)
{
    uint32_t old = pageBitmap[addr / 4096 / 32];
    pageBitmap[addr / 4096 / 32] &= ~(1 << ((addr / 4096) % 32));
    if (UNLIKELY(old == pageBitmap[addr / 4096 / 32])) {
        printk("attempted double physical page free: 0x%x", addr);
        kernelPanic("Halting.\n");
    }
    freePagesNum++;
}

