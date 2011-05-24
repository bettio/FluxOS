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
 *   Name: fbdevice.h                                                      *
 *   Date: 12/10/2010                                                      *
 ***************************************************************************/

#ifndef _FBDEVICE_H_
#define _FBDEVICE_H_

#include <stdint.h>

#define RGB_UINT32(r, g, b) ((uint32_t) r << 24 | (uint32_t) g << 16 | (uint32_t) b << 8)
#define UINT32_RED(r) (uint8_t) (r >> 24)
#define UINT32_GREEN(g) (uint8_t) (g >> 16)
#define UINT32_BLUE(b) (uint8_t) (b >> 8)

struct FBDevice
{
    int width;
    int height;
    int bpp;
    void *framebuffer;
    void (*clear)(FBDevice *fbdev, uint32_t bg);
    void (*drawMonoPixmap)(FBDevice *fbdev, uint8_t *pixmap, int w, int h, int x, int y, uint32_t bg, uint32_t fg);
    void (*fillRectange)(FBDevice *fbdev, int x, int y, int w, int h, uint32_t fg);
};

#endif
