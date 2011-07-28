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
 *   Name: inthandler.cpp                                                  *
 *   Date: 29/12/2004                                                      *
 ***************************************************************************/

#include <arch/ia32/core/inthandler.h>
#include <core/printk.h>

void IntHandler::IntNull()
{
    asm("cli");
    
    printk("Panic: Int Null !!!");
    
    while(1);
}

void IntHandler::Int0()
{
    asm("cli");

    printk("Panic: Int 0 - Divide Error !!");
    
    while(1);
}

void IntHandler::Int1()
{
    asm("cli");
    
    printk("Panic: Int 1 - Debug Interrupt !!");

    while(1);
}

void IntHandler::Int2()
{
    asm("cli");
    
    printk("Panic: Int 2  - NMI Interrupt !!");

    while(1);
}

void IntHandler::Int3()
{
    asm("cli");
    
    printk("Panic: Int 3  - Breakpoint !!");

    while(1);
}

void IntHandler::Int4()
{
    asm("cli");
    
    printk("Panic: Int 4  - Overflow !!");

    while(1);
}

void IntHandler::Int5()
{
    asm("cli");
    
    printk("Panic: Int 5  - BOUND Range Exceeded !!");

    while(1);
}

void IntHandler::Int6()
{
    asm("cli");
    
    printk("Panic: Int 6  - Invalid Opcode !!");

    while(1);
}

void IntHandler::Int7()
{
    asm("cli");
    
    printk("Panic: Int 7  - Device Not Avaible !!");

    while(1);
}

void IntHandler::Int8()
{
    asm("cli");
    
    printk("Panic: Int 8  - Double Fault !!");

    while(1);
}

void IntHandler::Int10()
{
    asm("cli");
    
    printk("Int 10  - Invalid TSS !!");

    while(1);
}

void IntHandler::Int11()
{
    asm("cli");
    
    printk("Int 11  - Segment Not Present !!");

    while(1);
}

void IntHandler::Int12()
{
    asm("cli");
    
    printk("Int 12  - Stack-Segment Fault !!");

    while(1);
}

void IntHandler::Int13()
{
    asm("cli");

    printk("Int 13  - General Protection !!");

    while(1);
}

void IntHandler::Int14()
{
    asm("cli");
    
    printk("Int 14  - Page Fault !!");

    while(1);
}

void IntHandler::Int16()
{
    asm("cli");
    
    printk("Int 16  - Floating-Point Error !!");

    while(1);
}

void IntHandler::Int17()
{
    asm("cli");
    
    printk("Int 17  - Alignment Check !!");

    while(1);
}

void IntHandler::Int18()
{
    asm("cli");
    
    printk("Int 18  - Machine Check !!");

    while(1);
}

void IntHandler::Int19()
{
    asm("cli");
    
    printk("Int 19  - Streaming SIMD Extensions !!");

    while(1);
}
