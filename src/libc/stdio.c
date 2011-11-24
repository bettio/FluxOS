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
 *   Name: stdio.c                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_DIGIT 9
#define FD(f) ((int) f)
#define FD_TO_FILE(f) ((FILE *) f)

int errno;
extern const char _string_syserrmsgs[];

int puts(const char *s)
{
	char newline = '\n';

	write(STDOUT_FILENO, s, strlen(s));

	return write(STDOUT_FILENO, (char *) &newline, 1);
}

int putchar(int c)
{
	return write(STDOUT_FILENO, (char *) &c, 1);
}

char *gets(char *c){
	/*int res = */read(STDIN_FILENO, c, 4096);

	/*if (res > 0) c[res - 1] = '\0';*/

	char *end = strchr(c, '\n');
	if (end != 0) *end = '\0';

	return 0;
}

FILE *fopen(const char *path, const char *mode)
{
	return FD_TO_FILE(open(path, 0, 0));
}

FILE *fdopen(int fildes, const char *mode)
{
	return FD_TO_FILE(fildes);
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
	return read(FD(stream), ptr, size*nmemb); //TODO: fix return value
}

size_t fwrite(const  void  *ptr,  size_t  size,  size_t  nmemb,  FILE *stream){
	return write(FD(stream), ptr, size*nmemb); //TODO: fix return value
}

int fputc(int c, FILE *stream){
	return write(FD(stream), (void *) &c, 1); //TODO: fix return value
}

int fputs(const char *s, FILE *stream){
	return write(FD(stream), s, strlen(s)); //TODO: fix return value
}

void perror(const char *s)
{
	char *errMsg = (char *) _string_syserrmsgs;
	int i = 0;

	while(i < errno){
		errMsg += strlen(errMsg) + 1;
		i++;
	}

	if (s){
		printf("%s: %s\n", s, errMsg);
	}else{
		puts(errMsg);
	}
}

//TODO: implement me
int fflush(FILE *stream)
{
	return 0;
}

int fgetc(FILE *stream)
{
	char c;

	read(FD(stream), &c, 1);

	return c;
}

char *fgets(char *s, int size, FILE *stream)
{
	/*int res = */read(FD(stream), s, size - 1);

	/*if (res > 0) c[res - 1] = '\0';*/

	char *end = strchr(s, '\n');
	if (end != 0) *++end = '\0';

	return s;
	//return 0;
}

int printf(const char *s, ...)
{
	va_list l;
	va_start(l, s);

	int retValue = vfprintf(stdout, s, l);

	va_end(l);

	return retValue;
}

int fprintf(FILE *stream, const char *s, ...)
{
	va_list l;
	va_start(l, s);

	int retValue = vfprintf(stream, s, l);

	va_end(l);

	return retValue;
}

int sprintf(char *tmpStr, const char *s, ...)
{
	va_list l;
	va_start(l, s);

	vsprintf(tmpStr, s, l);

	va_end(l);

	return strlen(tmpStr);
}

int vprintf(const char *s, va_list l)
{
	int retValue = vfprintf(stdout, s, l);

	return retValue;
}

int vfprintf(FILE *stream, const char *s, va_list l)
{
	char *str;
	char num[MAX_DIGIT + 1];
	int n;
	char p;
	int len;

	while (*s){
		switch (*s){
			case '%':
				switch (*(++s)){
					case 's':
						str = va_arg(l, char *);
		
						len = strlen(str);
						write(FD(stream), str, len);
		
						break;
		
					case 'c':
						p = (char) va_arg(l, int);
		
						write(FD(stream), &p, 1);
		
						break;
		
					case 'd':
					case 'i':
						n = va_arg(l, int);
						itoaz(n,num,10);
		
						len = strlen(num);
						write(FD(stream), num, len);
		
						break;
					case 'u':
						n = va_arg(l,unsigned int);
						uitoaz(n,num,10);
		
						len = strlen(num);
						write(FD(stream), num, len);
		
						break;
					case 'x':
						n = va_arg(l, int);
						uitoaz(n,num,16);
		
						len = strlen(num);
						write(FD(stream), num, len);
		
						break;
					}

				break;

			default:
				write(FD(stream), s, 1);
	
				break;
		}
		++s;
	}

	return 0; //TODO: return a value
}

int vsprintf(char *tmpStr, const char *s, va_list l)
{
	char *str;
	char num[MAX_DIGIT+1];
	int n;

	while (*s){
		switch (*s){
			case '%':
				switch (*(++s)){
					case 's':
						str = va_arg(l, char *);
		
						strcpy(tmpStr, str);
						tmpStr += strlen(str);
		
						break;
		
					case 'c':
						*tmpStr = (char) va_arg(l, int);
						tmpStr++;
		
						break;

					case 'd':
					case 'i':
						n = va_arg(l, int);
						itoaz(n,num,10);
		
						strcpy(tmpStr, num);
						tmpStr += strlen(num);
						
		
						break;
		
					case 'u':
						n = va_arg(l,unsigned int);
						uitoaz(n,num,10);
		
						strcpy(tmpStr, num);
						tmpStr += strlen(num);
		
						break;
		
					case 'x':
						n = va_arg(l, int);
						uitoaz(n,num,16);
		
						strcpy(tmpStr, num);
						tmpStr += strlen(num);
		
						break;
					}

					break;

			default:
				*tmpStr = *s;
				tmpStr++;
	
				break;
		}
		++s;
	}

	return strlen(tmpStr);
}

int fclose(FILE *fp)
{
	//ASSUME: EOF == -1
	return close(FD(fp));
}
