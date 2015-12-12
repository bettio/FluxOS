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
#include <arch.h>

#define ENABLE_DEBUG_MSG 0
#include <debugmacros.h>

#warning 32 bit only code

#ifdef ARCH_IA32
#define TARGET_MACHINE_TYPE EM_386
#endif
#ifdef ARCH_MIPS
#define TARGET_MACHINE_TYPE EM_MIPS
#endif

#if 0
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
#endif

int ElfLoader::loadExecutableFile(const char *path, LoadELFFlags flags)
{
    interpEntryPoint = NULL;

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

    if (!isValid()) {
        return -ENOEXEC;
    }

    ElfProgramHeader *pHeader = (ElfProgramHeader *) malloc(elfHeader->phentsize * elfHeader->phnum);
    res = FS_CALL(node, read)(node, elfHeader->phoff, (char *) pHeader, elfHeader->phentsize * elfHeader->phnum);
    if (res < 0){
        FileSystem::VNodeManager::PutVnode(node);
        return res;
    }

    MemoryContext *mContext = Scheduler::currentThread()->parentProcess->memoryContext;

    for (int i = 0; i < elfHeader->phnum; i++){
        switch (pHeader[i].type) {
          case ELF_PT_LOAD: {
            char *segment = (char *) pHeader[i].virtualAddr;
            if (((unsigned long) segment < USERSPACE_LOW_ADDR) || ((unsigned long) segment > USERSPACE_HI_ADDR)) {
                printk("ElfLoader: warning: skipping bad address: %p\n", segment);
                continue;
            }
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
                mContext->mapFileSegmentToMemory(node, segment, pHeader[i].segmentFileSize, pHeader[i].offset, permissions);
            }
            //TODO: we need to round up to page size everything here
            if (pHeader[i].segmentMemSize > pHeader[i].segmentFileSize){
                if (pHeader[i].segmentFileSize) {
                    printk("WARNING: not yet properly supported here\n");
                }
                mContext->allocateAnonymousMemory(((char *) segment) + pHeader[i].segmentFileSize, pHeader[i].segmentMemSize, permissions, MemoryContext::FixedHint);
            }
          }
          break;
          case ELF_PT_INTERP: {
              if (flags & FailOnInterpreter) {
                  return -ENOEXEC;
              }

              char *programInterpreter = (char *) malloc(pHeader[i].segmentMemSize);
              int res = FS_CALL(node, read)(node, pHeader[i].offset, programInterpreter, pHeader[i].segmentFileSize);
              DEBUG_MSG("ElfLoader::loadExecutableFile: program interpreter required: %s\n", programInterpreter);

              ElfLoader loader;
              res = loader.loadExecutableFile(programInterpreter, FailOnInterpreter);
              if (res < 0) {
                  printk("Cannot load executable interpreter: %s error: %i\n", programInterpreter, res);
                  return res;
              }
              interpEntryPoint = loader.entryPoint();
              return 0;
          }
          break;
          default: {
              DEBUG_MSG("Program header found, type: %i\n", pHeader[i].type);
          }
	}
    }

    FileSystem::VNodeManager::PutVnode(node);

    return 0;
}

bool ElfLoader::isValid() const
{
    if (!((elfHeader->ident[0] == 0x7F) &&
        (elfHeader->ident[1] == 'E') &&
        (elfHeader->ident[2] == 'L') &&
        (elfHeader->ident[3] == 'F'))) {
        printk("ELF loader:: ELF header magic is not valid: 0x%x 0x%x 0x%x 0x%x\n",
               elfHeader->ident[0], elfHeader->ident[1], elfHeader->ident[2], elfHeader->ident[3]);
        return false;
    }

    if (elfHeader->machine != TARGET_MACHINE_TYPE) {
        printk("ELF loader: Error: unsupported machine type: %i\n", elfHeader->machine);
        return false;
    }

    return true;
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

#if 0
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
#endif

void *ElfLoader::entryPoint()
{
    if (interpEntryPoint) {
        return interpEntryPoint;
    } else {
        //When the entry point is not defined we use the first defined function
        return (elfHeader->entry) ? (void *) elfHeader->entry : offsetToPtr(textOffset());
    }
}
