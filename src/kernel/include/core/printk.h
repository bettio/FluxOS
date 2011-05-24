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
 *   Name: printk.h                                                        *
 ***************************************************************************/

#ifndef CORE_PRINTK
#define CORE_PRINTK

#include <gccattributes.h>

struct CharDevice;
extern CharDevice *Out;

void printk(const char *str, ...) NON_NULL_ARG(1) FORMAT_STRING(printf, 1, 2);

void printk(bool value);

void printk(char c);
void printk(signed char c);
void printk(char c, int base);

void printk(int value);
void printk(unsigned int value);
void printk(unsigned int value, int base);

void printk(long long value);
void printk(unsigned long long value);
void printk(unsigned long long value, int base);

#endif
