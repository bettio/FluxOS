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
 *   Name: cmath.h                                                *
 *   Date: 07/08/2006                                                      *
 ***************************************************************************/


#ifndef _LIBC_CMATH_H
#define _LIBC_CMATH_H

#include <gccattributes.h>

extern "C"
{
	unsigned int uintpow(unsigned int x, unsigned int y) CONST_FUNCTION;

    unsigned long long ullpow(unsigned long long x, unsigned long long y) CONST_FUNCTION;

    int intpow(int x, int y) CONST_FUNCTION;
    
    long long llpow(long long x, long long y)  CONST_FUNCTION;
}

#endif
