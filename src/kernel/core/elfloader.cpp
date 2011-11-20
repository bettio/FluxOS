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
 *   Name: elfloader.cpp                                                   *
 *   Date: 21/11/2006                                                      *
 ***************************************************************************/

#include <core/elfloader.h>
#include <core/elf.h>
#include <filesystem/vfs.h>
#include <arch.h>
#include <core/printk.h>
#include <cstring.h>
#include <cstdlib.h>

#warning 32 bit only code

void ElfLoader::load(void *elfBinary)
{
    elfHeader = (Elf *) elfBinary;

    if (!isValid()){
        printk("Trying to run an invalid ELF binary: addr: %lx\n", (unsigned long) elfBinary);
        return;
    }

    for (int i = 0; i < elfHeader->shnum; i++){
        ElfShdr *section = sectionHeader(i);

        //BSS
        if (section->shType == NoBits){
            printk("Warning: ElfLoader: Unimplemented BSS support\n");

        //REL
        }else if (section->shType == Rel){
            ElfSymbol *syms = symbols();
            long text = textOffset();
            ElfRel *relocations = (ElfRel *) offsetToPtr(section->offset);

            for (unsigned int j = 0; j < section->size / sizeof(ElfRel); j++){
                int val = syms[relocations[j].info >> 8].sectionIndex;
                switch (relocations[j].info & 0xFF){
                    case R_386_32:
                        *((uint32_t *) offsetToPtr(text + relocations[j].offset)) += offsetToAddr(sectionHeader(val)->offset + syms[relocations[j].info >> 8].value);
                        break;
                        
                    case R_386_PC32:
                        *((int32_t *) offsetToPtr(text + relocations[j].offset)) += (sectionHeader(val)->offset + syms[relocations[j].info >> 8].value) - (text + relocations[j].offset);
                        break;
                       
                    default:
                        printk("Warning: ElfLoader: Unimplemented relocation type %i\n", relocations->info & 0xFF);
                    }
            }
        }else{
            continue;
        }
    }
}

bool ElfLoader::isValid() const
{
    return (elfHeader->ident[0] == 0x7F) &&
        (elfHeader->ident[1] == 'E') &&
        (elfHeader->ident[2] == 'L') &&
        (elfHeader->ident[3] == 'F');
}

long ElfLoader::textOffset()
{
    for (int i = 0; i < elfHeader->shnum; i++){
        ElfShdr *section = sectionHeader(i);

        if (section->flags & 0x0004){
            return section->offset;
        }
    }
    
    return 0;
}

ElfSymbol *ElfLoader::symbols()
{
    for (int i = 0; i < elfHeader->shnum; i++){
        ElfShdr *section = sectionHeader(i);

        if (section->shType == SymTab){
            return (ElfSymbol *) offsetToPtr(section->offset);
        }
    }
    
    return 0;
}

void *ElfLoader::entryPoint()
{
     //When the entry point is not defined we use the first defined function
    return (elfHeader->entry) ? (void *) elfHeader->entry : offsetToPtr(textOffset());
}
