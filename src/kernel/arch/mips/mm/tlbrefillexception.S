/***************************************************************************
 *   Copyright 2005 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: tlbrefillexception.S                                            *
 *   Date: 23/09/2005                                                      *
 ***************************************************************************/

#define NO_C_CODE 1

#include <cp0registers.h>
#include <arch/mips/core/cpuregistersframe.h>

.text
.globl tlbRefillException

tlbRefillException:
  checkUserOrKernel:
    mfc0 $k1, CP0_BadVAddr_Register
    srl $k0, $k0, 31
    beq $0, $k0, loadUserPageDirAddress

  loadKernelPageDirAddress:
    la $k0, kernelPageDir
    lw $k0, 0($k0)
    j loadPageDirEntry

  loadUserPageDirAddress:
    la $k0, userPageDir
    lw $k0, 0($k0)

  loadPageDirEntry:
    srl $k1, $k1, 22
    sll $k1, $k1, 2
    add $k0, $k0, $k1
    lw $k1, 0($k0) /* $k1 <- page dir entry */

  checkIsValid:
    andi $k1, $k1, 1
    beq $0, $k1, needsMoreProcessing

  probeForPageTableVAddr:
    lui $k0, 0xFFC0
    mfc0 $k1, CP0_BadVAddr_Register
    srl $k1, $k1, 12
    sll $k1, $k1, 2
    or $k0, $k0, $k1
    mtc0 $k0, CP0_EntryHi_Register
    tlbp
    nop
    nop
    mfc0 $k1, CP0_Index_Register
    srl $k1, $k1, 31
    beq $0, $k1, loadPageTableEntry

  pageDirEntryToTLB:
  choosekUserOrKernel:
    mfc0 $k1, CP0_BadVAddr_Register
    srl $k0, $k1, 31
    beq $0, $k0, loadUserPageDirAddressAgain

  loadKernelPageDirAddressAgain:
    la $k0, kernelPageDir
    lw $k0, 0($k0)
    j loadPageDirEntryAgain

  loadUserPageDirAddressAgain:
    la $k0, userPageDir
    lw $k0, 0($k0)

  loadPageDirEntryAgain:
    srl $k1, $k1, 22
    sll $k1, $k1, 2
    add $k0, $k0, $k1

/*
 * FIXME: Following code assume that the pair of entry is valid
 */
    srl $k0, $k0, 3
    sll $k0, $k0, 3
    lw $k1, 0($k0)
    srl $k1, $k1, 6
    ori $k1, $k1, 0x1 /* We should not force global here */
    ori $k1, $k1, 0x2 /* Valid */
    ori $k1, $k1, 0x4 /* We should not force dirty here */
    mtc0 $k1, CP0_EntryLo0_Register

    addi $k0, $k0, 4
    lw $k1, 0($k0)
    srl $k1, $k1, 6
    ori $k1, $k1, 0x1 /* We should not force global here */
    ori $k1, $k1, 0x2 /* Valid */
    ori $k1, $k1, 0x4 /* We should not force dirty here */
    mtc0 $k1, CP0_EntryLo1_Register

    lui $k0, 0xFFC0
    mfc0 $k1, CP0_BadVAddr_Register
    srl $k1, $k1, 23 /* 23? */
    sll $k1, $k1, 13 /* 13? */
    or $k0, $k0, $k1
    mtc0 $k0, CP0_EntryHi_Register
    tlbwr
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    /* some more nops here */

  loadPageTableEntry:
    lui $k0, 0xFFC0
    mfc0 $k1, CP0_BadVAddr_Register
    srl $k1, $k1, 12
    sll $k1, $k1, 2
    or $k0, $k0, $k1

  checkPTEIsValid:
    lw $k1, 0($k0)
    andi $k1, $k1, 1
    beq $0, $k1, needsMoreProcessing

  loadPageTableEntryToTLB:
    srl $k0, $k0, 3
    sll $k0, $k0, 3
    lw $k1, 0($k0)
    srl $k1, $k1, 6
    ori $k1, $k1, 0x1 /* We should not force global here */
    ori $k1, $k1, 0x2 /* Valid */
    ori $k1, $k1, 0x4 /* We should not force dirty here */
    mtc0 $k1, CP0_EntryLo0_Register

    addi $k0, $k0, 4
    lw $k1, 0($k0)
    srl $k1, $k1, 6
    ori $k1, $k1, 0x1 /* We should not force global here */
    ori $k1, $k1, 0x2 /* Valid */
    ori $k1, $k1, 0x4 /* We should not force dirty here */
    mtc0 $k1, CP0_EntryLo1_Register

    mfc0 $k0, CP0_BadVAddr_Register
    srl $k0, $k0, 13
    sll $k0, $k0, 13
    mtc0 $k0, CP0_EntryHi_Register
    tlbwr
    nop
    nop
    j interruptReturn

  needsMoreProcessing:
    j generalExceptionHandling
