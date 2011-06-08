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
 *   Name: serial.cpp                                                      *
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/

#include <drivers/chardevicemanager.h>
#include <arch/arm/drivers/serial.h>
#include <cstdlib.h>
#include <cstring.h>

volatile unsigned int * const UART0DR = (unsigned int *)0x101f1000;

CharDevice Serial::serial_tty = 
{
    0,
    4,
    1,
    Read,
    Write,
    read,
    write,
    ioctl,
    mmap,
    "tty1",
};

void Serial::Init()
{
    return;
}

void Serial::ReInit()
{
    CharDevice *tty = new CharDevice;
    //TODO: Warning: unchecked malloc
    tty->int_cookie = 0;
    tty->Major = 4;
    tty->Minor = 1;
    tty->name = "tty1";
    tty->Read = Read;
    tty->Write = Write;
    tty->read = read;
    tty->write = write;
    tty->ioctl = ioctl;

    CharDeviceManager::Register(tty);
}

CharDevice *Serial::Device()
{
    return &serial_tty;
}

int Serial::Write(CharDevice *cd, const char *buffer, int count)
{
    for (int i = 0; i < count; i++){
        *UART0DR = (unsigned int) buffer[i];
    }
    
    return count;
}

int Serial::Read(CharDevice *cd, char *buffer, int count)
{
    return 0;
}

int Serial::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
    for (unsigned int i = 0; i < bufsize; i++){
        *UART0DR = (unsigned int) buffer[i];
    }
    
    return bufsize;
}

int Serial::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    return 0;
}

int Serial::ioctl(VNode *node, int request, long arg)
{
    return -EINVAL;
}

void *Serial::mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return (void *) -ENODEV;
}
