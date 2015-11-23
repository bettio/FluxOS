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
#include <filesystem/vnodemanager.h>
#include <mm/memorycontext.h>
#include <task/scheduler.h>
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

void mapFileSegmentToMemory(VNode *node, void *virtualAddress, unsigned long length, unsigned long fileOffset, MemoryDescriptor::Permissions permissions)
{
    MemoryContext *mContext = Scheduler::currentThread()->parentProcess->memoryContext;

    MemoryMappedFileDescriptor *mappedFileDesc = new MemoryMappedFileDescriptor();
    mappedFileDesc->baseAddress = virtualAddress;
    mappedFileDesc->length = length;
    mappedFileDesc->permissions = permissions;
    mappedFileDesc->flags = MemoryDescriptor::MemoryMappedFile;
    mappedFileDesc->node = FileSystem::VNodeManager::ReferenceVnode(node);
    mContext->insertMemoryDescriptor(mappedFileDesc);
}

int ElfLoader::loadExecutableFile(const char *path)
{
    VNode *node;
    int res = FileSystem::VFS::RelativePathToVnode(0, path, &node);
    if (res < 0){
        return -ENOENT;
    }

    elfHeader = new Elf;
    res = FS_CALL(node, read)(node, 0, (char *) elfHeader, sizeof(Elf));
    if (res < 0){
        FileSystem::VNodeManager::PutVnode(node);
        return res;
    }

    if (!isValid()) printk("Not an ELF file\n");

    ElfProgramHeader *pHeader = (ElfProgramHeader *) malloc(elfHeader->phentsize * elfHeader->phnum);
    res = FS_CALL(node, read)(node, elfHeader->phoff, (char *) pHeader, elfHeader->phentsize * elfHeader->phnum);
    if (res < 0){
        FileSystem::VNodeManager::PutVnode(node);
        return res;
    }

    for (int i = 0; i < elfHeader->phnum; i++){
        if (pHeader[i].type == ELF_PT_LOAD){
            char *segment = (char *) pHeader[i].virtualAddr;
            #ifdef MIN_ELF_LOAD_ADDR
            if ((unsigned long) segment < MIN_ELF_LOAD_ADDR) {
                printk("ElfLoader: warning: skipping bad address: %p\n", segment);
                continue;
            }
            #endif
            MemoryDescriptor::Permissions permissions = MemoryDescriptor::NoAccess;
            if (pHeader[i].flags & ELF_PF_X) {
                permissions = (MemoryDescriptor::Permissions) (permissions | MemoryDescriptor::ExecutePermission);
            }
            if (pHeader[i].flags & ELF_PF_W) {
                permissions = (MemoryDescriptor::Permissions) (permissions | MemoryDescriptor::WritePermission);
            }
            if (pHeader[i].flags & ELF_PF_R) {
                permissions = (MemoryDescriptor::Permissions) (permissions | MemoryDescriptor::ReadPermission);
            }
            if (pHeader[i].segmentFileSize > 0) {
                mapFileSegmentToMemory(node, segment, pHeader[i].segmentFileSize, pHeader[i].offset, permissions);
            }
            //TODO: we need to round up to page size everything here
            if (pHeader[i].segmentMemSize > pHeader[i].segmentFileSize){
                if (pHeader[i].segmentFileSize) {
                    printk("WARNING: not yet properly supported here\n");
                }
                MemoryContext *mContext = Scheduler::currentThread()->parentProcess->memoryContext;
                mContext->allocateAnonymousMemory(((char *) segment) + pHeader[i].segmentFileSize, pHeader[i].segmentMemSize, permissions, MemoryContext::FixedHint);
            }
	}
    }

    FileSystem::VNodeManager::PutVnode(node);

    return 0;
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
