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
 *   Name: stat64.h                                                        *
 ***************************************************************************/

#ifndef _FILESYSTEM_STAT64_H_
#define _FILESYSTEM_STAT64_H_

#include <filesystem/fstypes.h>

#include <QHash>

struct stat64
{
    unsigned long long      st_dev;
    unsigned char   __pad0[4];

    unsigned long   __st_ino;

    unsigned int    st_mode;
    unsigned int    st_nlink;

    unsigned long   st_uid;
    unsigned long   st_gid;

    unsigned long long      st_rdev;
    unsigned char   __pad3[4];

    long long       st_size;
    unsigned long   st_blksize;

    /* 512 bytes blocks */
    unsigned long long      st_blocks;

    unsigned long   st_atime;
    unsigned long   st_atime_nsec;

    unsigned long   st_mtime;
    unsigned int    st_mtime_nsec;

    unsigned long   st_ctime;
    unsigned long   st_ctime_nsec;

    unsigned long long      st_ino;
};

#endif
