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
 *   Name: startup.s                                                       *
 *   Date: 27/09/2010                                                      *
 ***************************************************************************/

.global _start
_start:
 ldr sp, =stack_top
 bl main
 b .

.global swiHandler
swiHandler:
stmfd sp!, {r0-r12,lr}
mov r7, r7, lsl #2
ldr r8, =SyscallsTable
ldr r8, [r8, r7]
stmfd sp!, {r4-r6}
mov lr, pc
mov pc, r8
ldmfd sp!, {r4-r6}
ldmfd sp!, {r0-r12,lr}
movs pc, lr

.global irqHandler
irqHandler:
ldr sp, =0x800000
stmfd sp!, {r0-r12,lr}
stmfd sp, {sp}
ldmfd sp, {r0}
ldr r0, =0x10140030
mov lr, pc
ldr pc, [r0]
ldmfd sp!, {r0-r12,lr}
subs pc, r14, #4

.global fiqHandler
fiqHandler:
ldr sp, =0x800000
stmfd sp!, {r0-r12,lr}
stmfd sp, {sp}
ldmfd sp, {r0}
ldr r0, =0x10140030
mov lr, pc
ldr pc, [r0]
ldmfd sp!, {r0-r12,lr}
subs pc, r14, #4
