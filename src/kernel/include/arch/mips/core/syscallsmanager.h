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
 *   Name: syscallsmanager.h                                               *
 *   Date: 19/10/2015                                                      *
 ***************************************************************************/

#ifndef _SYSCALLSMANAGER_H_
#define _SYSCALLSMANAGER_H_

#include <stdint.h>

class SyscallsManager
{
    public:
        static void init();
        static void registerSyscall(int num, unsigned long (*func)(unsigned long a0, unsigned long a1, unsigned long a2));
        static void unregisterSyscall(int sysC);

    private:
        static void registerDefaultSyscalls();
};

#endif