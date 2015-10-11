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
 *   Name: boot.S                                                          *
 *   Date: 12/11/2015                                                      *
 ***************************************************************************/

#include <cp0registers.h>

#define STACK_SIZE 0x4000

.text
.globl __start
.globl haltloop

/*

  $a0: kernel arguments number
  $a1: kernel arguments table
  $a2: kernel enviornment table
  $a3: RAM size
 */
__start:
    /* small sanity check */
    beq $0, $a2, haltloop
    beq $0, $a3, haltloop

    /* configure EBase register */
    lui     $2, 0x8050
    mtc0    $2, CP0_EBase_Register, CP0_EBase_Select

    /* setupt stack */
    lui     $sp, %hi(stack + STACK_SIZE)
    addiu   $sp, $sp, %lo(stack + STACK_SIZE)

    /* jump to start */
    j yamonStart

haltloop:
    b haltloop

.data
.align 16
.comm   stack, STACK_SIZE
