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
 *   Name: cctype.h                                                        *
 *   Date: 01/02/2006                                                      *
 ***************************************************************************/


#ifndef _LIBC_CCTYPE_H
#define _LIBC_CCTYPE_H

#include <gccattributes.h>

extern "C"
{
	int isalnum(int ch) CONST_FUNCTION;
    
	int isalpha(int ch) CONST_FUNCTION;
    
	int iscntrl(int ch) CONST_FUNCTION;
    
	int isgraph(int ch) CONST_FUNCTION;
    
	int islower(int ch) CONST_FUNCTION;
    
	int isprint(int ch) CONST_FUNCTION;
    
	int ispunct(int ch) CONST_FUNCTION;
    
	int isspace(int ch) CONST_FUNCTION;
    
	int isupper(int ch) CONST_FUNCTION;
    
	int isxdigit(int ch) CONST_FUNCTION;
    
	int tolower(int ch) CONST_FUNCTION;
    
	int toupper(int ch) CONST_FUNCTION;
}

#endif
