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
 *   Name: exceptionsvector.cpp                                            *
 *   Date: 14/11/2015                                                      *
 ***************************************************************************/

#include <arch/mips/core/exceptionsvector.h>

#include <core/printk.h>

#define JUMP_OPCODE(addr) \
	0x08000000 | ((((uint32_t) addr) >> 2) & 0x0FFFFFFF)

extern "C" void generalExceptionsHandler();

extern "C" void panicISR(int cause)
{
    printk("PANIC: 0x%x (%i)\n", cause, cause);
    while(1);
}

void ExceptionsVector::init()
{
    installGeneralExceptionHandler();
}

void ExceptionsVector::installGeneralExceptionHandler()
{
    volatile unsigned long *generalExHandler = (unsigned long *) 0x80500180;
    *generalExHandler = JUMP_OPCODE(generalExceptionsHandler);
}
