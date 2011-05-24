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
 *   Name: gccbuiltins.h                                                   *
 *   Date: 08/2010                                                         *
 ***************************************************************************/

#ifndef _GCCBUILTINS_H
#define _GCCBUILTINS_H

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define READ_PREFETCH(addr) __builtin_prefetch(addr)
#define WRITE_PREFETCH(addr)  __builtin_prefetch(addr, 1)

//Temporal locality from 0 to 2
#define READ_PREFETCH_L(addr, locality) __builtin_prefetch(addr, 0, locality)
#define WRITE_PREFETCH_L(addr, locality)  __builtin_prefetch(addr, 1, locality)

#endif
