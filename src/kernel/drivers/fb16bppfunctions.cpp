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
 *   Name: fb16bppfunctions.cpp                                            *
 *   Date: 18/10/2010                                                      *
 ***************************************************************************/

#include <drivers/fb16bppfunctions.h>
#include <drivers/fbdevice.h>

void Fb16bppFunctions::drawMonoPixmap(FBDevice *fbdev, uint8_t *pixmap, int w, int h, int x, int y, uint32_t bg, uint32_t fg)
{
    int hRes = fbdev->width;
    uint8_t *framebuffer = (uint8_t *) fbdev->framebuffer;
    
    int vmemi = x*2+y*hRes*2;

    for(int pixb = 0; pixb < 16; pixb++){
        int pix = 0;

        for(pix = 7; pix >= 0; pix--){
            if (pixmap[pixb] & (1 << pix)){
                framebuffer[vmemi] = UINT32_RED(fg);
                framebuffer[vmemi+1] = UINT32_GREEN(fg);
                vmemi += 2;

            }else{
                framebuffer[vmemi] = UINT32_RED(bg);
                framebuffer[vmemi+1] = UINT32_GREEN(bg);
                vmemi += 2;
           }
        }

        vmemi = (y+pixb)*hRes*2+x*2;
    }
}

void Fb16bppFunctions::fillRectange(FBDevice *fbdev, int x, int y, int w, int h, uint32_t fg)
{
    int hRes = fbdev->width;
    uint8_t *framebuffer = (uint8_t *) fbdev->framebuffer;
    
    int vmemi = (x * 2) + (y * hRes * 2);

    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            framebuffer[vmemi] = UINT32_RED(fg);
            framebuffer[vmemi+1] = UINT32_GREEN(fg);
            vmemi += 2;
        }

        vmemi = ((y + i) * hRes * 2) + (x * 2);
    }
}

void Fb16bppFunctions::clear(FBDevice *fbdev, uint32_t bg)
{
    int msize = fbdev->width * fbdev->height;
    uint16_t *framebuffer = (uint16_t *) fbdev->framebuffer;

    for (int mvideo = 0; mvideo < msize; mvideo++){
        framebuffer[mvideo] = bg;
    }
}
