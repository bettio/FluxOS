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
 *   Name: arch.h                                                          *
 *   Date: 07/09/2006                                                      *
 ***************************************************************************/

#ifndef _ARCH_H_
#define _ARCH_H_

#define ARCH_IA32_NATIVE
#define ARCH_IA32

extern unsigned long kernel_heap_start;
extern unsigned long kernel_heap_end;

#define KERNEL_HEAP_START kernel_heap_start
#define KERNEL_HEAP_END kernel_heap_end

#define NEW_DEFAULT_TYPE unsigned int

#define LITTLE_ENDIAN 1234

#endif
