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
 *   Name: cstdlib.h                                                       *
 *   Date: 05/02/2006                                                      *
 ***************************************************************************/

#include <cstring.h>

extern "C"
{
	void initmem();
	void *calloc(size_t num, size_t size) MUST_CHECK ALLOC_FUNCTION ALLOC_SIZES(1, 2);
	void free(void *ptr) NON_NULL_ARGS;
	void *malloc(size_t size) MUST_CHECK ALLOC_FUNCTION ALLOC_SIZE(1);
	void *realloc(void *ptr, size_t size) MUST_CHECK ALLOC_SIZE(2) NON_NULL_ARGS;
}

int abs(int num) CONST_FUNCTION;
long abs(long num) CONST_FUNCTION;

extern "C"
{
	int atoi(const char *str);
	void itoaz (long long, char *, const unsigned int);
	void uitoaz(unsigned long long n, char s[], const unsigned int b);
	long atol(const char *str);
	long labs(long num) CONST_FUNCTION;
}
