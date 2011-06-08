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
 *   Name: exceptionsvector.cpp                                            *
 *   Date: 19/09/2010                                                      *
 ***************************************************************************/

#include <arch/arm/core/exceptionsvector.h>

#include <arch/arm/miscinstructions.h>
#include <stdint.h>

#define EXCEPTIONS_VECT ((volatile uint32_t *) (0x00000000))
#define HANDLERS_VECT ((volatile uint32_t *) (0x00000030))

#define BRANCH_TO_HANDLER_INSTRUCTION INCREMENT_PC(0x28)

#include <core/printk.h>

void ExceptionsVector::init()
{
    for (int i = 0; i < 8; i++){
        EXCEPTIONS_VECT[i] = INFINITE_LOOP_INSTRUCTION;    
    }
}

void ExceptionsVector::setHandler(ExceptionType i, void *handler)
{
    EXCEPTIONS_VECT[i] = BRANCH_TO_HANDLER_INSTRUCTION; 
    HANDLERS_VECT[i] = (uint32_t) handler;
}

void *ExceptionsVector::handler(ExceptionType i)
{
    return (void *) HANDLERS_VECT[i];
}
