/***************************************************************************
 *   Copyright 2015 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: inet.c                                                          *
 *   Date: 10/12/2015                                                      *
 ***************************************************************************/

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

int inet_pton(int af, const char *src, void *dst)
{
   int i = 0;
   int temp = 0;
   uint8_t ipv4[4];
   while (*src && (i < 4)) {
       if ((*src >= '0') && (*src <= '9')) {
           temp *= 10;
           temp += *src - '0';
       } else if (*src == '.') {
           ipv4[i] = temp;
           i++;
           temp = 0;
       } else {
           return 0;
       }
       src++;
   }
   memcpy(dst, ipv4, 4);
   return 1;
}


