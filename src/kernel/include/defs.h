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
 *   Name: defs.h                                                          *
 *   Date: 23/08/2010                                                      *
 ***************************************************************************/

#ifndef _DEFS_H_
#define _DEFS_H_

#define CHECK_THIS_OBJECT() \
          if (UNLIKELY(this == NULL)) return;

#define MAX(a, b) \
          ((a > b) ? a : b)

#define MIN(a, b) \
          ((a < b) ? a : b)

#define ABS(a) \
          ((a < 0) ? -a : a)

inline unsigned long alignToBound(unsigned long a, unsigned long alignTo) {return ((a + alignTo) / alignTo) * alignTo;}
          
#endif
