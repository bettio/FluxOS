/***************************************************************************
 *   Copyright 2012 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: registers.cpp                                                   *
 *   Date: 28/05/2012                                                      *
 ***************************************************************************/

#include <arch/arm/core/registers.h>

void enableInterrupts()
{
    asm("mrs r0, cpsr\n"
        "bic r0,r0,#0x80\n"
        "msr cpsr_c,r0\n"
        : : : "r0");
}

void setShadowStackRegister(ARMMode mode, void *sp)
{
    asm volatile(
        "mrs r0, cpsr\n"
        "bic r1, r0, #0x1F\n"
        "orr r1, r1, %1\n"
        "msr cpsr_c, r1\n"
        "mov sp, %0\n"
        "msr cpsr_c, r0\n"
        : : "r" (sp), "r" (mode) : "r0", "r1"
    );
}

