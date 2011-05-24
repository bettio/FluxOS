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
 *   Name: fbconsole.cpp                                                   *
 *   Date: 19/06/2005                                                      *
 ***************************************************************************/

#include <drivers/fbconsole.h> 
#include <drivers/consoledevice.h>
#include <drivers/vt.h>

#include "fbconsole_font.h"

#define screen_width (device->width / 8)
#define screen_height (device->height / 16)
#define screen_size (screen_height * screen_width)
#define tab_size 5

FBDevice *FBConsole::device;
uint16_t *FBConsole::charMem;
unsigned char FBConsole::CColor;
int FBConsole::Cy;
int FBConsole::Cx;

const char FBConsole::colors[16*3] =
{
    0, 0, 0,        //Black
    0, 0, 168,    //Blue
    0, 168, 0,    //Green
    0, 168, 168,    //Cyan
    168, 0, 0,    //Red
    168, 0, 168,    //Magenta
    168, 84, 0,    //Brown
    168, 168, 168,    //White
    84, 84, 84,    //Dark Grey
    84, 84, 252,    //Bright Blue
    84, 252, 84,    //Bright Green
    84, 252, 252,    //Bright Cyan
    252, 84, 84,    //Bright Red
    252, 84, 252,    //Bright Magenta
    252, 252, 84,    //Yellow
    252, 252, 252    //Bright White
};

CONSOLEDEVICE(consoledevice, FBConsole)

void FBConsole::init(FBDevice *dev)
{
    device = dev;
    charMem = (uint16_t *) malloc(screen_size*2);

    setTextColor(BrightWhite);
    setBackColor(Black);

    gotoXY(1,1);

    clear();

    Vt::init(&consoledevice);
}

void FBConsole::clear()
{
    unsigned int bColor = backColor();

    device->clear(device, RGB_UINT32(colors[(unsigned int)bColor*3],colors[(unsigned int)bColor*3+1],colors[(unsigned int)bColor*3+2]));

    for (int i = 0; i < screen_size; i++){
        charMem[i] = (unsigned char) ' ' | ((unsigned short) CColor << 8);
    }

    gotoXY(1,1);
}

void FBConsole::print(const char c)
{
    if (c == '\n'){
        NewLine();
        return;
    }

    if (c == '\t'){
        for(size_t i = 0; i < tab_size; i++){
            print(' ');
        }
        return;
    }

    if (Cx + 1 > screen_width){
        NewLine();
    }
    
    int charIndex = (Cx - 1) + (Cy - 1) * screen_width;
    charMem[charIndex] = c | ((unsigned short) CColor << 8);
    DrawChar(charMem[charIndex], Cx, Cy);

    gotoXY(Cx + 1, Cy);
}

void FBConsole::NewLine()
{
    if(Cy < screen_height){
        gotoXY(1, Cy + 1);

    }else if(Cy == screen_height){
        scroll();
    }
}

void FBConsole::scroll()
{
        for(int i = 0; i < screen_size - screen_width; i++){
            charMem[i] = charMem[i+screen_width];
        }

        for(int i = screen_size - screen_width; i <screen_size; i++){
            charMem[i] = (unsigned char) ' '  | ((unsigned short) CColor << 8) ;
        }

        for (int redrawY = 1; redrawY <= screen_height; redrawY++){
            for (int redrawX = 1; redrawX <= screen_width; redrawX++){
                if ((redrawY < 2) || (charMem[redrawX-1 +  (redrawY-1)*screen_width] != charMem[redrawX-1 +  (redrawY-2)*screen_width])){
                    DrawChar(charMem[redrawX-1 +  (redrawY-1)*screen_width],redrawX,redrawY);
                }
            }
        }

        gotoXY(1,screen_height); 
}


void FBConsole::gotoXY(int x, int y)
{
    if ((x <= screen_width) && (y <= screen_height)){    
        DrawChar(charMem[(Cx-1) + (Cy-1)*screen_width], Cx, Cy);

        Cx = x;
        Cy = y;

        DrawCursor(((unsigned short) CColor << 8), x, y);
    }
}

void FBConsole::setBackColor(Color bkColor)
{
    char mycolor = (char) bkColor;
    CColor &=0x8f;

    mycolor &= 0x7; 
    mycolor <<=4;

    CColor |= mycolor;
}

Color FBConsole::backColor()
{
    return (Color) ( (CColor & 0x7f) >>4 );
}

void FBConsole::setTextColor(Color txtColor)
{

    char mycolor = (char) txtColor;
    CColor = CColor & 0x70;
    CColor |= (mycolor & 0xF);
}

Color FBConsole::textColor()
{
    return (Color) ( CColor & 0xF );
}

int FBConsole::width()
{
    return screen_width; 
}

int FBConsole::height()
{
    return screen_height;
}

int FBConsole::x()
{
    return Cx;
}

int FBConsole::y()
{
    return Cy;
}

void FBConsole::setX(int x)
{
    gotoXY(x, Cy);
}

void FBConsole::setY(int y)
{
    gotoXY(Cx, y);
}


void FBConsole::DrawChar(unsigned short vchar, int vcharx, int vchary)
{
    int bcolor = ((vchar & 0xF000) >> 12);
    int tcolor = ((vchar & 0x0F00) >> 8);
    
    device->drawMonoPixmap(device, &vgafont16[(uint8_t)vchar*16], 8, 16, 8*(vcharx-1), 16*(vchary-1),
                            RGB_UINT32(colors[bcolor*3], colors[bcolor*3+1],colors[bcolor*3+2]),
                            RGB_UINT32(colors[tcolor*3], colors[tcolor*3+1], colors[tcolor*3+2]));
}

void FBConsole::DrawCursor(unsigned short vchar, int vcharx, int vchary)
{
    int tcolor = ((vchar & 0x0F00) >> 8);

    device->fillRectange(device, 8*(vcharx-1), 16*(vchary-1), 8, 16,
                            RGB_UINT32(colors[tcolor*3], colors[tcolor*3+1], colors[tcolor*3+2]));
}
