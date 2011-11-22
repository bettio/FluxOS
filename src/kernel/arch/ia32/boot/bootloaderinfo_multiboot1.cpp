/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: bootloaderinfo_multiboot1.cpp                                   *
 *   Date: 18/11/2011                                                      *
 ***************************************************************************/

#include <arch/ia32/boot/multibootinfo.h>
#include <boot/bootloaderinfo.h>
#include <arch/ia32/boot/multiboot.h>
#include <defs.h>
#include <core/elf.h>

multiboot_info *MultiBootInfo::infoBlock;

unsigned long kernel_heap_start;
unsigned long kernel_heap_end;
    
bool MultiBootInfo::init(unsigned long magic, multiboot_info *info)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC){
        return false;
    }

    infoBlock = info;

    //Calculate the address where free mem start
    unsigned long maxAddr = 0;
    maxAddr = MAX(maxAddr, (unsigned long) infoBlock);
    if (infoBlock->flags & (1 << 2)){
        maxAddr = MAX(maxAddr, infoBlock->cmdline);
    }

    if (infoBlock->flags & (1 << 3)){
        maxAddr = MAX(maxAddr, infoBlock->mods_addr + sizeof(module_t) * infoBlock->mods_count);
        module_t *modules = (module_t *) infoBlock->mods_addr;
        for (unsigned long i = 0; i < infoBlock->mods_count; i++){
            maxAddr = MAX(maxAddr, modules[i].mod_end);
        }
    }

    if (infoBlock->flags & (1 << 5)){
        maxAddr = MAX(maxAddr, infoBlock->u.elf_sec.addr + infoBlock->u.elf_sec.num * infoBlock->u.elf_sec.size);
        ElfShdr *sections = (ElfShdr *) infoBlock->u.elf_sec.addr;
        for (unsigned long i = 0; i < infoBlock->u.elf_sec.num; i++){
            maxAddr = MAX(maxAddr, sections[i].addr + sections[i].size);
        }
    }

    if (infoBlock->flags & (1 << 6)){
        maxAddr = MAX(maxAddr, infoBlock->mmap_addr);
        memory_map *mm = (memory_map *) infoBlock->mmap_addr;
        
        unsigned long maxAvailAreaSize = 0;
        unsigned long maxAvailAreaIndex = 0;
        for (unsigned long i = 0; i < infoBlock->mmap_length / sizeof(memory_map); i++){
            if ((mm[i].type == 1) && (maxAvailAreaSize < mm[i].length_low)){
                maxAvailAreaIndex = i;
                kernel_heap_end = mm[maxAvailAreaIndex].base_addr_low + mm[i].length_low;
            }
        }
        maxAddr = MAX(maxAddr, mm[maxAvailAreaIndex].base_addr_low);
    }

    //Round up to page boundary
    kernel_heap_start = alignToBound(maxAddr, 4096);

    return true;
}

void *BootLoaderInfo::module(int i)
{
    return (void *) (((module_t *) MultiBootInfo::infoBlock->mods_addr)[i].mod_start);
}

int BootLoaderInfo::modulesCount()
{
    return (MultiBootInfo::infoBlock->flags & (1 << 3)) ? MultiBootInfo::infoBlock->mods_count : 0;
}
