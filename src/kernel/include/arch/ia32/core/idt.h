/***************************************************************************
 *   Copyright 2004 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: idt.h                                                           *
 *   Date: 28/12/2004                                                      *
 ***************************************************************************/

#ifndef _IA32_CORE_IDT_H_
#define _IA32_CORE_IDT_H_

#include <stdint.h>
#include <cstring.h>

#define IDT_SIZE 256

class IDT{
    public:
        static void init();
        static void setHandler(void (*func)(), int index, int dpl = 0);
        static void setIDTR(volatile uint64_t *base, int numDesc);
    private:
        static volatile uint64_t idt[IDT_SIZE];
};

#endif
