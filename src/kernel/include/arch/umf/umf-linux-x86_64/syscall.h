/***************************************************************************
 *   Copyright 2008 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: syscalls.h                                                      *
 *   Date: 16/07/2008                                                      *
 ***************************************************************************/

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <kdef.h>
#include <cstring.h>

class SysCall{
	public:
		static void Init();
		static void Add(uint64_t (*func)(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi), int sysC);
		static void Remove(int sysC);

		static uint64_t NullSysCallHandler(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi);

		static uint64_t (*SysCallTable[256])(uint64_t ebx, uint64_t ecx, uint64_t edx, uint64_t esi, uint64_t edi);
};

#endif
