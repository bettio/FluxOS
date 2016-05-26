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

#ifndef _ELFLOADER_H_
#define _ELFLOADER_H_

#include <core/elf.h>
#include <stddef.h>

struct AuxData;

class ElfLoader
{
    enum LoadELFFlags {
        NoLoadELFFlags = 0,
        FailOnInterpreter = 1
    };

    public:
        void load(void *elfBin);
        int loadExecutableFile(const char *path, AuxData *auxData, LoadELFFlags = NoLoadELFFlags);
        bool isValid() const;
        void *entryPoint();

    private:
       Elf *elfHeader;
       ElfSymbol *symbols();
       long textOffset();
       inline long baseAddr() { return (long) elfHeader; }
       inline void *offsetToPtr(long offset) { return (void *) (((long) elfHeader) + offset); }
       inline long offsetToAddr(long offset) { return (((long) elfHeader) + offset); }
       inline ElfShdr *sectionHeader(int i) { return (ElfShdr *) offsetToPtr(elfHeader->shoff + elfHeader->shentsize * i); }
       void *interpEntryPoint;
};

#endif
