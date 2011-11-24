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
 *   Name: memfails.cpp                                                    *
 *   Date: 29/05/2011                                                      *
 ***************************************************************************/

#include <core/printk.h>

extern "C"
{
    void mallocAbort()
    {
        printk("malloc abort\n");
        while(1);
    }
    
    void mallocFail()
    {
        printk("malloc fail\n");
        while(1);        
    }
    
    void mallocUsageError(void *ptr)
    {
        printk("malloc usage error\n");
        while(1);
    }

    void mallocCorruptionError(void *ptr)
    {
        printk("malloc corruption error\n");
        while(1);
    }
}
