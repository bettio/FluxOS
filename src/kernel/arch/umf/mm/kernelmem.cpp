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
 *   Name: kernelmem.cpp                                                   *
 *   Date: 25/05/2011                                                      *
 ***************************************************************************/

#include <arch/umf/core/hostsyscalls.h>

extern "C"
{
    void *mmap(void *addr, size_t length, int prot, int flags, int fd, long offset)
    {
        return HostSysCalls::mmap(addr, length, prot, flags, fd, offset);
    }

    int munmap(void *addr, size_t length)
    {
        return HostSysCalls::munmap(addr, length);
    }
}
