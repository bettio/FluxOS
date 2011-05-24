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
 *   Name: fb24bppfunctions.cpp                                            *
 *   Date: 18/10/2010                                                      *
 ***************************************************************************/

#include <drivers/fb24bppfunctions.h>
#include <drivers/fbdevice.h>

void Fb24bppFunctions::drawMonoPixmap(FBDevice *fbdev, uint8_t *pixmap, int w, int h, int x, int y, uint32_t bg, uint32_t fg)
{
    int hRes = fbdev->width;
    uint8_t *framebuffer = (uint8_t *) fbdev->framebuffer;
    
    int vmemi = x*3+y*hRes*3;

    for(int pixb = 0; pixb < 16; pixb++){
        int pix = 0;

        for(pix = 7; pix >= 0; pix--){
            if (pixmap[pixb] & (1 << pix)){
                framebuffer[vmemi] = UINT32_RED(fg);
                framebuffer[vmemi+1] = UINT32_GREEN(fg);
                framebuffer[vmemi+2] = UINT32_BLUE(fg);
                vmemi += 3;

            }else{
                framebuffer[vmemi] = UINT32_RED(bg);
                framebuffer[vmemi+1] = UINT32_GREEN(bg);
                framebuffer[vmemi+2] = UINT32_BLUE(bg);
                vmemi += 3;
           }
        }

        vmemi = (y+pixb)*hRes*3+x*3;
    }
}

void Fb24bppFunctions::fillRectange(FBDevice *fbdev, int x, int y, int w, int h, uint32_t fg)
{
    int hRes = fbdev->width;
    uint8_t *framebuffer = (uint8_t *) fbdev->framebuffer;
    
    int vmemi = (x * 3) + (y * hRes * 3);

    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            framebuffer[vmemi] = UINT32_RED(fg);
            framebuffer[vmemi+1] = UINT32_GREEN(fg);
            framebuffer[vmemi+2] = UINT32_BLUE(fg);
            vmemi += 3;
        }

        vmemi = ((y + i) * hRes * 3) + (x * 3);
    }
}

void Fb24bppFunctions::clear(FBDevice *fbdev, uint32_t bg)
{
    int msize = fbdev->width * fbdev->height;
    uint8_t *framebuffer = (uint8_t *) fbdev->framebuffer;

    for (int mvideo = 0; mvideo < msize; mvideo++){
        framebuffer[mvideo] = bg;
    }
}
