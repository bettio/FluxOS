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
 *   Name: elf.h                                                           *
 *   Date: 21/11/2006                                                      *
 ***************************************************************************/

#ifndef _ELF_H_
#define _ELF_H_

#include <stdint.h>

struct Elf
{
    unsigned char ident[16];
    uint16_t fileType;
    uint16_t machine;
    uint32_t version;
    uint32_t entry; //Addr
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
};

enum SectionHeaderType
{
    Null = 0,
    ProgBits = 1,
    SymTab = 2,
    StrTab = 3,
    Rela = 4,
    Hash = 5,
    Dynamic = 6,
    Note = 7,
    NoBits = 8,
    Rel = 9,
    ShLib = 10,
    DynSym = 11,
    LoProc = 0x70000000,
    HiProc = 0x7FFFFFFF,
    LoUser = 0x80000000,
    HiUser = 0x7FFFFFFF
};

struct ElfShdr
{
    uint32_t shName;
    uint32_t shType;
    uint32_t flags;
    uint32_t addr;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t addralign;
    uint32_t entsize;
};

struct ElfProgramHeader
{
    uint32_t type;
    uint32_t offset;
    uint32_t virtualAddr;
    uint32_t physicalAddr;
    uint32_t segmentFileSize;
    uint32_t segmentMemSize;
    uint32_t flags;
    uint32_t align;
};

struct ElfRel
{
    uint32_t offset;
    uint32_t info;
};

#define R_386_32 1
#define R_386_PC32 2

struct ElfSymbol
{
    uint32_t name;
    uint32_t value;
    uint32_t size;
    uint8_t info;
    uint8_t other;
    uint16_t sectionIndex;
};

#define ELF_PT_LOAD 1

#define ELF_PF_X 1
#define ELF_PF_W 2
#define ELF_PF_R 4

/* Machine types */
#define EM_SPARC 2
#define EM_386 3
#define EM_MIPS 8
#define EM_PPC 20
#define EM_PPC64 21
#define EM_ARM 40
#define EM_SPARCV9 43
#define EM_X86_64 62
#define EM_AARCH64 183

#endif
