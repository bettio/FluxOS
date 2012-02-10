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
 *   Name: cstring.cpp                                                     *
 *   Date: 19/09/2010                                                      *
 ***************************************************************************/

#include <string.h>
#include <stdlib.h>

#define LIKELY(x) (x)

#if 0
extern "C"
{
#endif

    void *memchr(const void *s, int c, size_t n)
    {
        size_t i;
        for (i = 0; i < n; i++){
            if  (((const unsigned char *)s)[i] == ((unsigned char) c)){
                return (((unsigned char *)s) + i);
            }
        }
        
        return 0;
    }

    void *memrchr(const void *s, int c, size_t n)
    {
        size_t i;
        for (i = n - 1; i >= 0; i--){
            if  (((const unsigned char *)s)[i] == ((unsigned char) c)){
                return (((unsigned char *)s) + i);
            }
        }
        
        return 0;      
    }

    int memcmp(const void *s1, const void *s2, size_t n)
    {
        size_t i;
        for (i = 0; i < n; i++){
            if (((const unsigned char *)s1)[i] != ((const unsigned char *)s2)[i]){
                return (((const unsigned char *)s1)[i] < ((const unsigned char *)s2)[i]) ? -1 : 1;
            }
        }
        
        return 0;
    }

    void *memcpy(void *dest, const void *src, size_t n)
    {
        size_t i;
        for (i = 0; i < n; i++){
            ((char *) dest)[i] = ((const char *) src)[i];
        }
        
        return dest;
    }

    void *memmove(void *dest, const void *src, size_t n)
    {
        size_t i;
        if (dest > src){
            for (i = n - 1; i >= 0; i--){
                ((char *) dest)[i] = ((const char *) src)[i];
            }
        }else if (dest < src){
            for (i = 0; i < n; i++){
                ((char *) dest)[i] = ((const char *) src)[i];
            }
        }

        return dest;
    }

    void *memset(void *s, int c, size_t n)
    {
        size_t i;
        for (i = 0; i < n; i++){
            ((unsigned char *) s)[i] = c; 
        }
        
        return s;
    }

    char *strcat(char *dest, const char *src)
    {
        size_t i = strlen(dest);
    
        while (*src != '\0'){
            dest[i] = *src;
            i++;
            src++;
        }
        
        dest[i] = '\0';
    
        return dest;
    }

    char *strncat(char *dest, const char *src, size_t n)
    {
        size_t destI = strlen(dest);
        size_t i;
        
        for (i = 0; i < n; i++){
            if (src[i] == '\0'){
                break;
            }

            dest[destI] = src[i];
        }
        dest[destI] = '\0';
        
        return dest;
    }

    char *strchr(const char *s, int c)
    {
        while(*s != '\0'){
            if (((unsigned char) *s) == ((unsigned char) c)){
                return (char *) s;
            }
        
            s++;
        }

        return NULL;
    }

    char *strrchr(const char *s, int c)
    {
        const char *lastOccurrence = NULL;
        
        while(*s != '\0'){
            if (((unsigned char) *s) == ((unsigned char) c)){
                lastOccurrence = s;
            }

            s++;
        }

        return (char *) lastOccurrence;
    }

    int strcmp(const char *str1, const char *str2)
    {
        int i = 0;
        while (str1[i] == str2[i]){
            if (str1[i] == '\0'){
                return 0; 
            }
            i++;
        }

        return (((unsigned char) str1[i]) < ((unsigned char) str2[i])) ? -1 : 1;
    }

    int strncmp(const char *str1, const char *str2, size_t n)
    {
        size_t i;
        for (i = 0; i < n; i++){
            if (str1[i] != str2[i]){
                return (((unsigned char) str1[i]) < ((unsigned char) str2[i])) ? -1 : 1;
            }
            
            if (str1[i] == '\0'){
                return 0; 
            }
        }
        
        return 0;
    }

    char *strcpy(char *dest, const char *src)
    {
        size_t i = 0;
        while (src[i] != '\0'){
            dest[i] = src[i];
            i++;
        }
        
        dest[i] = '\0';
        
        return dest;
    }

    size_t strlcpy(char *dst, const char *src, size_t size)
    {
        size_t i;
        for (i = 0; i < size - 1; i++){
            dst[i] = src[i];
            
            if (src[i] == '\0'){
                return i;
            }
         }
         
         dst[i] = '\0';
         
         while (src[i] != '\0'){
             i++;
         }
         return i;
    }

    char *strncpy(char *dest, const char *src, size_t n)
    {
        size_t i;
        for (i = 0; i < n; i++){
            if (src[i] == '\0'){
                break;
            }
        
            dest[i] = src[i];
        }
        
        for (; i < n; i++){
            dest[i] = '\0';
        }
        
        return dest;
    }

    char *strdup(const char *s)
    {
        int len = strlen(s);
        char *newstr = (char *) malloc(len + 1);
        //if (LIKELY(newstr)){
            memcpy(newstr, s, len + 1);
        //}

        return newstr;
    }

    char *strndup(const char *s, size_t n)
    {
        size_t slen = strlen(s);
        size_t minLen = (n > slen) ? slen : n;
        char *newstr = (char *) malloc(minLen + 1);
        //if (LIKELY(newstr)){
            memcpy(newstr, s, minLen);
            newstr[minLen] = '\0';
        //}

        return newstr;
    }

    size_t strlen(const char *s)
    {
        size_t i = 0;
        while (s[i] != '\0'){
            i++;
        }
        
        return i;
    }

    size_t strnlen(const char *s, size_t maxlen)
    {
        size_t i = 0;
        while ((i < maxlen) && (s[i] != '\0')){
            i++; 
        }

        return i;
    }
    
    char *rawstrcpy(char *dest, const char *src, size_t destSize, size_t srcSize)
    {
        size_t n = (srcSize < destSize) ? srcSize : destSize;

        size_t i;
        for (i = 0; i < n - 1; i++){        
            dest[i] = src[i];
            
            if (src[i] == '\0'){
                return dest;
            }
        }
        
        dest[i] = '\0';
        
        return dest;
    }

#if 0
}
#endif
