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
 *   Name: nulldev.cpp                                                     *
 *   Date: 13/12/2011                                                      *
 ***************************************************************************/

#include <drivers/nulldev.h>
#include <drivers/chardevice.h>

CharDevice NullDev::nullDev =
{
    0,
    7,
    1,
    Read,
    Write,
    read,
    write,
    ioctl,
    mmap,
    "null",
};

void NullDev::init()
{
    CharDeviceManager::Register(&nullDev);
}

int NullDev::Write(CharDevice *cd, const char *buffer, int count)
{
    return count;
}

int NullDev::Read(CharDevice *cd, char *buffer, int count)
{
    return 0;
}

int NullDev::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
    return Write(NULL, buffer, bufsize);
}

int NullDev::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    return Read(NULL, buffer, bufsize);
}

int NullDev::ioctl(VNode *node, int request, long arg)
{
    return 0;
}

void *NullDev::mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return 0;
}

