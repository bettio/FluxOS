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
 *   Name: stdio.h                                                         *
 ***************************************************************************/

#ifndef _STDIO_H
#define _STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EOF
# define EOF (-1)
#endif

#ifndef _SIZE_T_TYPE
#define _SIZE_T_TYPE
	typedef unsigned int size_t;
#endif

typedef unsigned int FILE;

#define stdout ((FILE *) STDOUT_FILENO)
#define stdin ((FILE *) STDIN_FILENO)
#define stderr ((FILE *) STDERR_FILENO)

extern void perror(const char *s);

extern int puts(const char *s);
extern int putchar(int c);
extern int printf(const char *format, ...);
extern char *gets(char *s);

extern FILE *fopen(const char *path, const char *mode);
extern FILE *fdopen(int fildes, const char *mode);

extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern size_t fwrite(const  void  *ptr,  size_t  size,  size_t  nmemb,  FILE *stream);

extern int fputc(int c, FILE *stream);
extern int fputs(const char *s, FILE *stream);

extern int fflush(FILE *stream);
extern char *fgets(char *s, int size, FILE *stream);
extern int fgetc(FILE *stream);


extern int fprintf(FILE *stream, const char *format, ...);


extern int sprintf(char *str, const char *format, ...);
extern int fclose(FILE *fp);

#include <stdarg.h>

extern int vprintf(const char *format, va_list ap);
extern int vfprintf(FILE *stream, const char *format, va_list ap);
extern int vsprintf(char *str, const char *format, va_list ap);


#if 0
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
void rewind(FILE *stream);
int fgetpos(FILE *stream, fpos_t *pos);
int fsetpos(FILE *stream, fpos_t *pos);
int fscanf(FILE *stream, const char *format, ...);
int fclose(FILE *fp);
#endif

#ifdef __cplusplus
}
#endif

#endif
