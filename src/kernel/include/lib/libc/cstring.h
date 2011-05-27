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
 *   Name: cstring.h                                                       *
 *   Date: 19/09/2010                                                      *
 ***************************************************************************/

#ifndef _CSTRING_H_
#define _CSTRING_H_

#include <gccattributes.h>

typedef unsigned int size_t;

#if __cplusplus
extern "C"
{
#endif
    void *memchr(const void *buffer, int ch, size_t count) NON_NULL_ARGS;

    int memcmp(const void *buf1, const void *buf2, size_t count) MUST_CHECK NON_NULL_ARGS;
    
    void *memcpy(void *to, const void *from, size_t count) NON_NULL_ARGS;
    
    void *memmove(void *to, const void *from, size_t count) NON_NULL_ARGS;
    
    void *memset(void *buf, int ch, size_t count) NON_NULL_ARGS;

    char *strcat(char *dest, const char *src) NON_NULL_ARGS;
    
    char *strchr(const char *s, int c) NON_NULL_ARGS;
    
    char *strrchr(const char *s, int c) NON_NULL_ARGS;
    
    char *strcpy(char *dest, const char *src) NON_NULL_ARGS;
    
    size_t strcspn(const char *str1, const char *str2) NON_NULL_ARGS;
    
    char *strncat(char *dest, const char *src, size_t n) NON_NULL_ARGS;
    
    char *strncpy(char *dest, const char *src, size_t n) NON_NULL_ARGS;
    
    char *strpbrk(const char *str1, const char *str2) NON_NULL_ARGS;
    
    char *strrchr(const char *str, int ch) NON_NULL_ARGS;
    
    size_t strspn(const char *str1, const char *str2) NON_NULL_ARGS;
    
    char *strstr(const char *str1, const char *str2) NON_NULL_ARGS;
    
    char *strtok(char *str1, const char *str2) NON_NULL_ARGS;

    int strcmp(const char *str1, const char *str2) MUST_CHECK NON_NULL_ARGS;
    
    int strncmp(const char *str1, const char *str2, size_t n) MUST_CHECK NON_NULL_ARGS;
    
    char *strdup(const char *s) MUST_CHECK ALLOC_FUNCTION NON_NULL_ARGS;
    
    char *strndup(const char *s, size_t n) MUST_CHECK ALLOC_FUNCTION NON_NULL_ARGS;
    
    size_t strnlen(const char *str, size_t maxlen) NON_NULL_ARGS;
    
    size_t strlen(const char *str) NON_NULL_ARGS;
    
    size_t strlcpy(char *dst, const char *src, size_t size) NON_NULL_ARGS;
    
    char *rawstrcpy(char *dest, const char *src, size_t destSize, size_t srcSize) NON_NULL_ARGS;
#if __cplusplus
}
#endif

#endif
