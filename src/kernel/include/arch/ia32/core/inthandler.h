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
 *   Name: inthandler.h                                                    *
 *   Date: 29/12/2004                                                      *
 ***************************************************************************/

#ifndef _IA32_CORE_INTHANDLER_H_
#define _IA32_CORE_INTHANDLER_H_

class IntHandler
{
    public:
        static void IntNull();
        static void Int0(); //#DE - Divide Error
        static void Int1(); //#DB - Debug
        static void Int2(); //NMI Interrupt
        static void Int3(); //#BP - Breakpoint
        static void Int4(); //#OF - Overflow
        static void Int5(); //#BR - BOUND Range Exceeded
        static void Int6(); //#UD - Invalid Opcode
        static void Int7(); //#NM - Device Not Avaible
        static void Int8(); //#DF - Double Fault
        static void Int10(); //#TS - Invalid TSS
        static void Int11(); //#NP - Segment Not Present
        static void Int12(); //#SS - Stack-Segment Fault
        static void Int13(); //#GP - General Protection
        static void Int14(); //#PF - Page Fault
        static void Int16(); //#MF - Floating-Point Error
        static void Int17(); //#AC - Alignment Check
        static void Int18(); //#MC - Machine Check
        static void Int19(); //#XF - Streaming SIMD Extensions
};

#endif
