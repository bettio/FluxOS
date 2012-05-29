/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/

#ifndef _ARCH_H_
#define _ARCH_H_

#define ARCH_ARM

#define NEW_DEFAULT_TYPE unsigned int

extern unsigned long kernel_heap_free_pos;
#define KERNEL_HEAP_START 0x2800000
#define KERNEL_HEAP_END 0x4600000
#define KERNEL_HEAP_FREE_POS kernel_heap_free_pos
#define KERNEL_SPACE_UPPER_LIMIT 0x4600000

#define LITTLE_ENDIAN 1234

#endif
