/***************************************************************************
 *   Copyright 2005 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: printk.cpp                                                      *
 ***************************************************************************/

#include <core/printk.h>
#include <drivers/chardevicemanager.h>
#include <cstring.h>
#include <cstdarg>
#include <cstdlib.h>

#include <QMutex>

#define MAX_DIGIT 9

CharDevice *Out;
QMutex printkMutex;

void printk(const char *s, ...)
{
	va_list l;
	va_start(l,s);

	char *str, num[MAX_DIGIT+1];
	int n;
	char p;
	int len;

    printkMutex.lock();

	while (*s){
		switch (*s){
		case '%':
			switch (*(++s)){
			case 's':
				str = va_arg(l, char *);

				len = strlen(str);
				Out->Write(Out, str, len);

				break;

			case 'c':
				p = (char) va_arg(l, int);

				Out->Write(Out, &p, 1);

				break;

			case 'i':
				n = va_arg(l, int);
				itoaz(n,num,10);

				len = strlen(num);
				Out->Write(Out, num, len);

				break;
			case 'u':
				n = va_arg(l,unsigned int);
				uitoaz(n,num,10);

				len = strlen(num);
				Out->Write(Out, num, len);

				break;
			case 'x':
				n = va_arg(l, int);
				uitoaz(n,num,16);

				len = strlen(num);
				Out->Write(Out, num, len);

				break;
			}
			break;
		default:
			Out->Write(Out, s, 1);

			break;
		}
		++s;
	}

    printkMutex.unlock();

	va_end(l);
}

void printk(bool value)
{
	if (value){
		Out->Write(Out, "true", 4);
	}else{
		Out->Write(Out, "false", 5);
	}
}

void printk(char c)
{
	Out->Write(Out, &c, 1);
}

void printk(int value)
{
	char num[MAX_DIGIT+1];
	itoaz(value, num, 10);

	int len = strlen(num);
	Out->Write(Out, num, len);
}

void printk(unsigned int value)
{
	char num[MAX_DIGIT+1];
	itoaz(value, num, 10);

	int len = strlen(num);
	Out->Write(Out, num, len);
}

void printk(long long value)
{
	char num[MAX_DIGIT+1];
	itoaz((int) value, num, 10);

	int len = strlen(num);
	Out->Write(Out, num, len);
}

void printk(unsigned long long value)
{
	char num[MAX_DIGIT+1];
	itoaz((int) value, num, 10);

	int len = strlen(num);
	Out->Write(Out, num, len);
}
