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
 *   Name: stdlib.h                                                        *
 *   Date: 05/02/2006                                                      *
 ***************************************************************************/

#ifndef _STDLIB_H
#define _STDLIB_H

#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0

#define TRUE 1
#define FALSE 0

#define NULL 0

#ifndef _SIZE_T_TYPE
#define _SIZE_T_TYPE
	typedef unsigned int size_t;
#endif

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

extern int atoi(const char *s);
extern void itoaz(const int n, char *s, const unsigned int b);
extern void uitoaz(const unsigned int n, char s[], const unsigned int b);

extern void *calloc(size_t nmemb, size_t size);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern void *realloc(void *ptr, size_t size);

extern void exit(int status);

#endif
