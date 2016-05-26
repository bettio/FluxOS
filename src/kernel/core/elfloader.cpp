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
#include <task/auxdata.h>
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

int ElfLoader::loadExecutableFile(const char *path, AuxData *auxData, LoadELFFlags flags)
{
    interpEntryPoint = NULL;

    VNode *node;
    int res = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, path, &node);
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
        FileSystem::VNodeManager::PutVnode(node);
        return -ENOEXEC;
    }

    ElfProgramHeader *pHeader = (ElfProgramHeader *) malloc(elfHeader->phentsize * elfHeader->phnum);
    res = FS_CALL(node, read)(node, elfHeader->phoff, (char *) pHeader, elfHeader->phentsize * elfHeader->phnum);
    if (res < 0){
        FileSystem::VNodeManager::PutVnode(node);
        return res;
    }

    MemoryContext *mContext = Scheduler::currentThread()->parentProcess->memoryContext;

    bool firstPTLoad = true;

    for (int i = 0; i < elfHeader->phnum; i++){
        switch (pHeader[i].type) {
          case ELF_PT_LOAD: {
            char *segment = (char *) pHeader[i].virtualAddr;

            if (((unsigned long) segment < USERSPACE_LOW_ADDR) || ((unsigned long) segment > USERSPACE_HI_ADDR)) {
                printk("ElfLoader: warning: skipping bad address: %p\n", segment);
                continue;
            }

            // keep track of interpreter base address so we can export it to the aux vector
            if ((flags & FailOnInterpreter) && (auxData->interpreterBaseAddress == 0)) {
                auxData->interpreterBaseAddress = (unsigned long) segment;
            } else if (firstPTLoad) {
                firstPTLoad = false;
                // not an interpreter
                auxData->interpreterBaseAddress = 0;
                auxData->entryPointAddress = (unsigned long) entryPoint();
                auxData->programHeaderAddress = ((unsigned long) segment) + elfHeader->phoff;
                auxData->programHeaderEntriesCount = elfHeader->phnum;
                auxData->programHeaderEntrySize = elfHeader->phentsize;
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

            if ((pHeader[i].segmentFileSize > 0) && (pHeader[i].segmentMemSize == pHeader[i].segmentFileSize)) {
                mContext->mapFileSegmentToMemory(node, segment, pHeader[i].segmentFileSize, pHeader[i].offset, permissions);
            }
            //TODO: we need to round up to page size everything here
            if (pHeader[i].segmentMemSize > pHeader[i].segmentFileSize) {
                if (pHeader[i].segmentFileSize) {
                   //TODO: replace 4096 with page size
                   if (pHeader[i].segmentFileSize > 4096) {
                       printk("ElfLoader::loadExecutableFile: not yet full tested 1\n");
                       mContext->mapFileSegmentToMemory(node, segment, pHeader[i].segmentFileSize & ~0xFFF, pHeader[i].offset, permissions);
                   }

                   mContext->allocateDelayedLoadedFile(((char *) segment) + (pHeader[i].segmentFileSize & ~0xFFF), pHeader[i].segmentMemSize,
                                                       node, pHeader[i].offset + (pHeader[i].segmentFileSize & ~0xFFF), 0,
                                                       pHeader[i].segmentFileSize % 4096, permissions, MemoryContext::FixedHint);

                   if (pHeader[i].segmentMemSize - pHeader[i].segmentFileSize > 4096) {
                       printk("ElfLoader::loadExecutableFile: not yet fully tested 2\n");
                       mContext->allocateAnonymousMemory(((char *) segment) + (pHeader[i].segmentFileSize & ~0xFFF) + 0x1000,
                                                         pHeader[i].segmentMemSize - (pHeader[i].segmentFileSize & ~0xFFF) + 0x1000,
                                                         permissions, MemoryContext::FixedHint);
                   }

                } else {
                    mContext->allocateAnonymousMemory(((char *) segment), pHeader[i].segmentMemSize, permissions, MemoryContext::FixedHint);
                }
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
              res = loader.loadExecutableFile(programInterpreter, auxData, FailOnInterpreter);
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

void *ElfLoader::entryPoint()
{
    if (interpEntryPoint) {
        return interpEntryPoint;
    } else {
        //When the entry point is not defined we use the first defined function
        return (elfHeader->entry) ? (void *) elfHeader->entry : offsetToPtr(textOffset());
    }
}
