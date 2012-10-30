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
 *   Name: fulldev.cpp                                                     *
 *   Date: 13/12/2011                                                      *
 ***************************************************************************/

#include <drivers/fulldev.h>
#include <drivers/chardevice.h>
#include <errors.h>

CharDevice FullDev::fullDev =
{
    0,
    5,
    1,
    Read,
    Write,
    read,
    write,
    ioctl,
    mmap,
    "full",
};

void FullDev::init()
{
    CharDeviceManager::Register(&fullDev);
}

int FullDev::Write(CharDevice *cd, const char *buffer, int count)
{
    return -ENOSPC;
}

int FullDev::Read(CharDevice *cd, char *buffer, int count)
{
    for (int i = 0; i < count; i++){
        buffer[i] = 0;
    }

    return count;
}

int FullDev::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
    return Write(NULL, buffer, bufsize);
}

int FullDev::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    return Read(NULL, buffer, bufsize);
}

int FullDev::ioctl(VNode *node, int request, long arg)
{
    return 0;
}

void *FullDev::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return 0;
}


