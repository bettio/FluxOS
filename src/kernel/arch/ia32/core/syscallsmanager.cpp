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
 *   Name: syscallsmanager.cpp                                             *
 *   Date: 09/07/2005                                                      *
 ***************************************************************************/

#include <arch/ia32/core/syscallsmanager.h>

#include <arch/ia32/core/idt.h>
#include <core/printk.h>
#include <filesystem/vfs.h>

#define SYSCALL_MAXNUM 256

extern "C" void syscallHandler();

uint32_t (*SyscallsTable[SYSCALL_MAXNUM])(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);

void SyscallsManager::init()
{
    IDT::setHandler(syscallHandler, 128, 0 /* RING_0 */); //TODO: change to ring3

    for(int i = 0; i < SYSCALL_MAXNUM; i++){
        registerSyscall(i, nullSysCallHandler);
    }
    registerDefaultSyscalls();
}

void SyscallsManager::registerSyscall(int num, uint32_t (*func)(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi))
{
    SyscallsTable[num] = func;
}

void SyscallsManager::unregisterSyscall(int num)
{
    SyscallsTable[num] = nullSysCallHandler;
}

uint32_t SyscallsManager::nullSysCallHandler(uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    printk("Null System Call Handler:\n"
           " - ebx: 0x%x, ecx: 0x%x\n"
           " - edx: 0x%x, esi: 0x%x\n"
           " - edi: 0x%x",
           ebx, ecx, edx, esi, edi);

    return -EFAULT;
}
    
extern "C" void doSyscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t esp)
{
    if (eax < 256){
        SyscallsTable[eax](ebx, ecx, edx, esi, edi);
    
    }else{
         SyscallsManager::nullSysCallHandler(ebx, ecx, edx, esi, edi);
    }
}

asm(
    ".globl syscallHandler\n"
    "syscallHandler:\n"
    "cli\n"
    "pusha\n"
    "pushl %esp\n"
    "pushl %edi\n"
    "pushl %esi\n"
    "pushl %edx\n"
    "pushl %ecx\n"
    "pushl %ebx\n"
    "pushl %eax\n"
    "call doSyscall\n"
    "addl $28, %esp\n"
    "popa\n"
    "sti\n"
    "iret\n"
);

void SyscallsManager::registerDefaultSyscalls()
{
}
