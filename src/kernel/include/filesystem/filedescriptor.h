/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: filedescriptor.h                                                *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#ifndef _FILESYSTEM_FILEDESCRIPTOR_H
#define _FILESYSTEM_FILEDESCRIPTOR_H

#define O_ACCMODE      0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#define O_CREAT		   0100

class FileDescriptor
{
    public:
        FileDescriptor();
        FileDescriptor(VNode *vn);

        VNode *node;
        uint64_t fpos;

        int flags;
};

#endif
