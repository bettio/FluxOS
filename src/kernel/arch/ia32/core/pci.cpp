/***************************************************************************
 *   Copyright 2011 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: pci.cpp                                                         *
 *   Date: 26/07/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/core/pci.h>
#include <arch/ia32/io.h>
#include <core/printk.h>

#include <arch/ia32/drivers/bochsfb.h>
#include <arch/ia32/drivers/rtl8139.h>

void PCI::init()
{
    for (int bus = 0; bus < 1; bus++){
        for (int slot = 0; slot < 32; slot++){
            uint32_t id = read(bus, slot, 0, 0);
            if (id != 0xFFFFFFFF){
                switch (id){
                    case 0x813910EC:
                        rtl8139::init(bus, slot);
                        break;

                    case 0x11111234:
                        BochsFB::init(bus, slot);
                        break;

                    default:
                        printk("pci: id: %x:%x, bus: %i, slot: %i, iobase: %x, irq: %i\n", id & 0xFFFF, (id >> 16), bus, slot,
                               read(bus, slot, 0, PCI_IOBASE), read(bus, slot, 0, 0x3C) & 0xF);
                }
            }
        }
    }
}

uint32_t PCI::read(int bus, int slot, int function, int reg)
{
    outport32(0xCF8, 1 << 31 | bus << 16 | slot << 11 |  function << 8 | reg);
    return inport32(0xCFC);
}
