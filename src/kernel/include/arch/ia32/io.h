/***************************************************************************
 *   Copyright 2003 by Abruzzo Silvio, 2011 by Bettio Davide               *
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
 *   Name: io.h                                                            *
 ***************************************************************************/

#ifndef _IA32_IO_H_
#define _IA32_IO_H_

#include <stdint.h>

inline uint8_t inportb_p(uint16_t port)
{
    uint8_t ret;

    asm volatile ("inb %%dx,%%al;outb %%al,$0x80":"=a" (ret):"d" (port));

    return ret;
}

inline void outportb_p(uint16_t port, uint8_t value)
{
    asm volatile ("outb %%al,%%dx;outb %%al,$0x80": :"d" (port), "a"(value));
}

inline void outportw_p(uint16_t port, uint16_t value)
{
    asm volatile ("outw %%ax,%%dx;outb %%al,$0x80": :"d" (port), "a"(value));
}

inline uint8_t inportb(uint16_t port)
{
    uint8_t ret;

    asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));

    return ret;
}

inline void outportb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %%al,%%dx": :"d" (port), "a"(value));
}

inline void outport16(uint16_t port, uint16_t value)
{
    asm volatile ("outw %%ax,%%dx": :"d" (port), "a"(value));
}

inline uint16_t inport16(uint16_t port)
{
    uint16_t _v;

    asm volatile ("inw %%dx,%%ax":"=a" (_v):"d"(port));

    return _v;
}

inline void outport32(uint16_t port, uint32_t value)
{
    asm volatile ("outl %%eax,%%dx": :"d" (port), "a"(value));
}

inline uint32_t inport32(uint16_t port)
{
    uint32_t v;

    asm volatile ("inl %%dx,%%eax":"=a" (v):"d"(port));

    return v;
}

#define outb(value, address) outportb(address, value)
#define inb(address) inportb(address)
#define inw(address) inportw(address)

#endif
