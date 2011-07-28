/***************************************************************************
 *   Copyright 2004 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: video.cpp                                                       *
 *   Date: 16/09/2004                                                      *
 ***************************************************************************/

#include <arch/ia32/drivers/video.h>

#include <arch/ia32/io.h>
#include <drivers/vt.h>
#include <cstring.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define SCREEN_SIZE (SCREEN_HEIGHT*SCREEN_WIDTH)

#define tab_size 5

unsigned short *Video::videomem;    
unsigned char Video::CColor;
int Video::Cy;
int Video::Cx;

CONSOLEDEVICE(Video::consoleDevice, Video)

void Video::init()
{
    videomem = (unsigned short *) 0xB8000;

    setTextColor(White);
    setBackColor(Black);

    gotoXY(1,1);

    clear();

    Vt::init(&consoleDevice);
}

void Video::clear()
{
    for (int i = 0; i < SCREEN_SIZE; i++){
        videomem[i] = (unsigned char) ' ' | ((unsigned short) CColor << 8);
    }

    ///gotoXY(1,1);
}

void Video::print(const char c)
{
    static bool gotonewline = false;

    if(c == '\n'){
        NewLine();
        gotonewline = false;

        return;
    }

    if(c == '\t'){
        for(size_t i = 0; i < tab_size; i++){
            print(' ');
        }
        return;
    }

    if(gotonewline == true && Cx == SCREEN_WIDTH){
        NewLine();
        gotonewline = false;
    
    }else if(gotonewline == true && Cx != SCREEN_WIDTH){
        gotonewline = false;
    }    

    if((Cx - 1 + (Cy - 1) * SCREEN_WIDTH) == SCREEN_SIZE){
        for(size_t i=0; i<SCREEN_SIZE - SCREEN_WIDTH; i++){
            videomem[i] = videomem[i+SCREEN_WIDTH];
        }
        
        for(int i = SCREEN_SIZE - SCREEN_WIDTH; i < SCREEN_SIZE; i++){
            videomem[i] = (unsigned char) ' '  | ((unsigned short) CColor << 8) ;
        
        }
        
        gotoXY(1, SCREEN_HEIGHT);
    }
    
    videomem[(Cx-1) + (Cy-1) * SCREEN_WIDTH] = (unsigned char) c | ((unsigned short) CColor << 8);

    if(Cx == SCREEN_WIDTH){
        gotonewline = true;
    }else{
        gotoXY(Cx + 1, Cy);
    }
}

void Video::NewLine()
{
    if(Cy < SCREEN_HEIGHT){
        gotoXY(1, Cy + 1);

    }else if(Cy == SCREEN_HEIGHT){
        for(int i = 0; i < SCREEN_SIZE - SCREEN_WIDTH; i++){
            videomem[i] = videomem[i+SCREEN_WIDTH];
        }

        for(int i = SCREEN_SIZE - SCREEN_WIDTH; i < SCREEN_SIZE; i++){
            videomem[i] = (unsigned char) ' '  | ((unsigned short) CColor << 8) ;
        }

        gotoXY(1, SCREEN_HEIGHT);
    }
}

void Video::gotoXY(int x, int y)
{
    if( (x <= SCREEN_WIDTH && y <= SCREEN_HEIGHT)  &&  (x>0 && y>0) ){
        Cx = x;
        Cy = y;
    
        uint32_t offset = (Cx-1) + (Cy-1)*SCREEN_WIDTH;

        outportb(0x3D4, 0x0F);
        outportb(0x3D5,(uint8_t) offset);

        outportb(0x3D4, 0x0E);
        outportb(0x3D5,(uint8_t) (offset >> 8));
    }
}

void Video::setX(int x)
{
    gotoXY(x, Cy);
}

void Video::setY(int y)
{
    gotoXY(Cx, y);
}

void Video::setBackColor(Color bkColor)
{
    char mycolor = (char) bkColor;
    CColor &= 0x8F;
    mycolor &= 0x7;
    mycolor <<= 4;
    CColor |= mycolor;
}

Color Video::backColor()
{
    return (Color) ((CColor & 0x7f) >> 4);
}

void Video::setTextColor(Color txtColor)
{
    char mycolor = (char) txtColor;
    CColor = CColor & 0x70;
    CColor |= (mycolor & 0xF);
}

Color Video::textColor(){

    return (Color) ( CColor & 0xF );
}

int Video::width()
{
    return SCREEN_WIDTH;
}

int Video::height()
{
    return SCREEN_HEIGHT;
}

int Video::x()
{
    return Cx;
}

int Video::y()
{
    return Cy;
}
