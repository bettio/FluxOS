/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
 *   Copyright 2006 by Marco Pagliaricci                                   *
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
 *   Name: cstdlib.cpp                                                     *
 *   Date: 05/02/2006                                                      *
 ***************************************************************************/

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>

#define MAX_DIGIT 9

void abort()
{
    printf("abort");
    _exit(1);
}

void *sbrk(intptr_t increment)
{
    unsigned long oldProgramBreak = brk(0);
    brk((void *) oldProgramBreak + increment);
    return (void *) oldProgramBreak;
}

int atoi(const char *s)
{
	int base = 10, f = 0, retn = 0, i = (strlen(s) - 1);

	if (*s == '-'){
		f = 1;
		++s;
	}

	if (*s == '0'){
		++s;
		--i;
		if (*s == 'x'){
			base = 16;
			--i;
			++s;
		}else if ((*s > '0') && (*s <= '9')){
			base = 8;
		}
	}

	while (*s){
		if ((*s >= '0') && (*s <= '9')){
			retn = retn + (pow(base,i) * ((*s) - '0'));

		}else if ((*s >= 'A') && (*s <= 'F')) {
			retn = retn + (pow(base,i) * ((*s) - ('A' - 10)));

		}else{
			retn /= base;
		}

		s++;
		i--;
	}

	if (f == 1) {
		return -retn;
	}else{
		return retn;
	}
}

void itoaz(const int n, char *s, const unsigned int b)
{
	int i = 0, l, r, k, j, index = 0, sign = 0;
	char p[MAX_DIGIT] = { 0 };

	if (n < 0){
		l = -n;
		sign = 1;
	}else{
		l = n;
	}

	while(1){
		r = l % b;
		l = l / b;

		if ((r >= 0) && (r <= 9)){
			p[i] = '0' + r;
		}else if (r > 9){
			p[i] = 'A' + r - 10;
		}

		++i;

		if (l <= 0) break;
	}

	if (sign == 1){
		s[index] = '-';
		++index;
	}

	for (k = i, j = index; k > 0; --k, ++j){
		s[j] = p[k-1];
	}

	s[j] = '\0';
}

void uitoaz(const unsigned int n, char s[], const unsigned int b)
{
	int i = 0, l, r, k, j, index = 0, sign = 0;
	char p[MAX_DIGIT] = { 0 };

	if (n < 0){
		l = -n;
		sign = 1;
	}else{
		l = n;
	}

	while (1){
		r = l % b;
		l = l / b;

		if ((r >= 0) && (r <= 9)){
			p[i] = '0' + r;
		}else if (r > 9){
			p[i] = 'A' + r - 10;
		}

		++i;

		if (l <= 0) { break; }
	}

	if (sign == 1){
		s[index] = '-';
		++index;
	}

	for (k = i, j = index; k > 0; --k, ++j){
		s[j] = p[k-1];
	}

	s[j] = '\0';
}

long atol(const char *str)
{

	return 0;
}

int abs(int num)
{
	if (num < 0) return -num;

	return num;
}

long int labs(long int num)
{
	if (num < 0) return -num;

	return num;
}

long long int llabs(long long int num)
{
	if (num < 0) return -num;

	return num;
}

void exit(int status)
{
	_exit(status);
}

//C++ new operator
void *_Znwj(unsigned int s)
{
	return malloc(s);
}

int isatty(int fd)
{
    struct stat s;
    int ret = fstat(fd, &s);
    if (ret < 0) {
       return 0;
    }

    if (S_ISCHR(s.st_mode)) {
        return 1;

    } else {
        errno = -ENOTTY;
        return 0;
    }
}

extern unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
    //TODO STUB
#warning strtoul not implemented
}
