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
 *   Name: ps2mouse.cpp                                                    *
 *   Date: 18/12/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/drivers/ps2mouse.h>

#include <arch/ia32/core/irq.h>
#include <arch/ia32/io.h>
#include <core/printk.h>
#include <drivers/chardevicemanager.h>

uint8_t mouseBytes[3];
bool readyBytes;

CharDevice PS2Mouse::mouseDev =
{
    0,
    22,
    1,
    0,
    0,
    read,
    0,
    0,
    0,
    "psaux",
};

void PS2Mouse::waitController()
{
    uint8_t data = 0x02;

    while (data & 0x02){
        data = inportb(0x64);
    }
}

uint8_t PS2Mouse::readPort0x60(){
    uint8_t data = 0;
    while (!(data & 0x01)){
        data = inportb(0x64);
    }
    return inportb(0x60);
}

void PS2Mouse::writePort0x60(uint8_t value)
{
    waitController();
    outportb(0x60, value);
}

void PS2Mouse::writePort0x64(uint8_t value)
{
    waitController();
    outportb(0x64, value);
}

void PS2Mouse::init()
{
    writePort0x64(0xAD);

    writePort0x64(0x20);
    uint8_t status = readPort0x60();
    status |= 0x02;
    writePort0x64(0x60);
    writePort0x60(status);

    writePort0x64(0xD4);
    writePort0x60(0xF4);

    writePort0x64(0xAE);

    CharDeviceManager::Register(&mouseDev);

    IRQ::setHandler(mouseInterruptHandler, 12);
    IRQ::enableIRQ(12);
}

void PS2Mouse::mouseInterruptHandler()
{
    uint8_t status = inportb(0x64);
    if (!((status & 1) && (status & 0x20))){
        return;
    } 
    mouseBytes[0] = inportb(0x60);
    mouseBytes[1] = inportb(0x60);
    mouseBytes[2] = inportb(0x60);
    readyBytes = true;
}

int PS2Mouse::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    int rb = 0;

    for (unsigned int i = 0; i < bufsize; i += 3){
        while (!readyBytes);
        int readSize = (bufsize - rb > 3) ? 3 : bufsize - rb;
        memcpy(buffer + i, mouseBytes, readSize);
        readyBytes = false;
        rb += readSize;
    }

    return rb;
}

