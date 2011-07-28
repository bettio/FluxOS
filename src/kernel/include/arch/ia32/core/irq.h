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
 *   Name: irq.h                                                           *
 *   Date: 30/12/2004                                                      *
 ***************************************************************************/

#ifndef _IA32_CORE_IRQ_H_
#define _IA32_CORE_IRQ_H_

#include <cstring.h>
#include <stdint.h>


class IRQ
{
    public:
        static void init();
        static void enableIRQ(int irq_no);
        static void disableIRQ(int irq_no);			
        static void setHandler(void (*func) (), uint8_t irq);
        static void (*handler[16])();
        static int currentIRQ();
        static void endOfIRQ(int irq);

    private:
        static void nullHandler();
        static uint16_t irqMask;
        static void init8259();
};

#endif

