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
 *   Name: chardevice.h                                                    *
 *   Date:  27/12/2006                                                     *
 ***************************************************************************/

#ifndef _DRIVERS_CHARDEVICE_H
#define _DRIVERS_CHARDEVICE_H

#include <filesystem/fstypes.h>
#include <filesystem/vnode.h>

struct CharDevice
{
    unsigned int int_cookie;

    int Major;
    int Minor;

    int (*Read) (CharDevice *cd, char *buffer, int count);
    int (*Write) (CharDevice *cd, const char *buffer, int count);

    int (*read) (VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
    int (*write) (VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize);
    int (*ioctl)(VNode *node, int request, long arg);
    void *(*mmap)(void *start, size_t length, int prot, int flags, int fd, off_t offset); //TODO -Eerror?

    const char *name;
};

#endif
