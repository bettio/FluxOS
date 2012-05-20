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

#define ENABLE_PRINTK_MSGS 1
#define MAX_DIGIT 21

CharDevice *Out;
QMutex printkMutex;

void printk(const char *s, ...)
{
#if ENABLE_PRINTK_MSGS
	va_list l;
	va_start(l,s);

	char num[MAX_DIGIT+1];
	int len;

    printkMutex.lock();

	while (*s){
		switch (*s){
		case '%':
			switch (*(++s)){
			case 's':{
				char *str = va_arg(l, char *);

				len = strlen(str);
				Out->Write(Out, str, len);

				break;
			}
			case 'c':{
				char ch = (char) va_arg(l, int);

				Out->Write(Out, &ch, 1);

				break;
			}
			case 'i':{
				int n = va_arg(l, int);
				itoaz(n,num,10);

				len = strlen(num);
				Out->Write(Out, num, len);

				break;
			}
			case 'u':{
				unsigned int n = va_arg(l,unsigned int);
				uitoaz(n,num,10);

				len = strlen(num);
				Out->Write(Out, num, len);

				break;
			}
			case 'x':{
				unsigned int n = va_arg(l, unsigned int);
				uitoaz(n,num,16);

				len = strlen(num);
				Out->Write(Out, num, len);

				break;
			}
            case 'l':
                switch (*(++s)){
                case 'i':{
                    long ln = va_arg(l, long);
                    itoaz(ln,num,10);

                    len = strlen(num);
                    Out->Write(Out, num, len);

                    break;
                }
                case 'u':{
                    unsigned long uln = va_arg(l,unsigned long);
                    uitoaz(uln,num,10);

                    len = strlen(num);
                    Out->Write(Out, num, len);

                    break;
                }
                case 'x':{
                    unsigned long uln = va_arg(l, unsigned long);
                    uitoaz(uln,num,16);

                    len = strlen(num);
                    Out->Write(Out, num, len);

                    break;
                }
                case 'l':
                    switch (*(++s)){
                    case 'i':{
                        long long lln = va_arg(l, long long);
                        itoaz(lln,num,10);

                        len = strlen(num);
                        Out->Write(Out, num, len);

                        break;
                    }
                    case 'u':{
                        unsigned long long ulln = va_arg(l,unsigned long long);
                        uitoaz(ulln,num,10);

                        len = strlen(num);
                        Out->Write(Out, num, len);

                        break;
                    }
                    case 'x':{
                        unsigned long long ulln = va_arg(l, unsigned long long);
                        uitoaz(ulln,num,16);

                        len = strlen(num);
                        Out->Write(Out, num, len);

                        break;
                    }
                    }
                }
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
#endif
}

void printk(bool value)
{
#if ENABLE_PRINTK_MSGS
	if (value){
		Out->Write(Out, "true", 4);
	}else{
		Out->Write(Out, "false", 5);
	}
#endif
}

void printk(char c)
{
#if ENABLE_PRINTK_MSGS
	Out->Write(Out, &c, 1);
#endif
}

void printk(int value)
{
#if ENABLE_PRINTK_MSGS
	char num[MAX_DIGIT+1];
	itoaz(value, num, 10);

	int len = strlen(num);
	Out->Write(Out, num, len);
#endif
}

void printk(unsigned int value)
{
#if ENABLE_PRINTK_MSGS
	char num[MAX_DIGIT+1];
	itoaz(value, num, 10);

	int len = strlen(num);
	Out->Write(Out, num, len);
#endif
}

void printk(long long value)
{
#if ENABLE_PRINTK_MSGS
	char num[MAX_DIGIT+1];
	itoaz(value, num, 10);

	int len = strlen(num);
	Out->Write(Out, num, len);
#endif
}

void printk(unsigned long long value)
{
#if ENABLE_PRINTK_MSGS
	char num[MAX_DIGIT+1];
	uitoaz(value, num, 10);

	int len = strlen(num);
	Out->Write(Out, num, len);
#endif
}

