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
 *   Name: registers.h                                                     *
 *   Date: 28/05/2012                                                      *
 ***************************************************************************/

#ifndef _ARM_CORE_REGISTERS_H_
#define _ARM_CORE_REGISTERS_H_

enum ARMMode
{
    UserMode = 0x10,
    FIQMode = 0x11,
    IRQMode = 0x12,
    SupervisorMode = 0x13,
    AbortMode = 0x17,
    UndefinedMode = 0x1B,
    SystemMode = 0x1F
};

void enableInterrupts();
void setShadowStackRegister(ARMMode mode, void *sp);

#endif

