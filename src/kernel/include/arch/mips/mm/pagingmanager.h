/***************************************************************************
 *   Copyright 2015 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: pagingmanager.h                                                 *
 *   Date: 10/20/2015                                                      *
 ***************************************************************************/

#ifndef _ARCH_MIPS_PAGINGMANAGER_H_
#define _ARCH_MIPS_PAGINGMANAGER_H_

#include <stdint.h>

class PagingManager
{
    public:
        enum PageFlags
        {
            Present = 1,
            Write = 2,
            User = 4,
        };

        static void init();
        static void removePages(void *addr, unsigned long len);
        static void newPage(uint32_t addr, unsigned long flags = 0);
};

#endif
