/***************************************************************************
 *   Copyright 2015 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: userprocessimage.h                                              *
 *   Date: 09/11/2015                                                      *
 ***************************************************************************/

#ifndef _USERPROCESSIMAGE_H_
#define _USERPROCESSIMAGE_H_

#include <QList>
#include <mm/usermemoryops.h>

extern "C" void schedule();

struct AuxData;
struct ProcessControlBlock;

class UserProcessImage
{
    public:
        static int setupInitProcessImage();
        static int execve(userptr const char *filename, userptr char *const argv[], userptr char *const envp[]);

    private:
        static void buildNewEnvironment(userptr const char *const env[], int envCount, userptr char *envTable[], userptr char *envBlock);
        static void buildNewArgsList(userptr const char *const args[], int argsCount, userptr char *argsTable[], userptr char *argsBlock);
        static int copyUserspaceStringsVectorToBlock(char *destStringsBlock, userptr const char *const srcStringsVect[], int destStringsBlockSize);
        static void buildStringsPtrVector(userptr char *destArgsTable[], userptr char *destArgsBlock, const char *srcArgsBlock, int argsCount);
        static int stringsVectorSize(userptr char *const v[], int *num, int maxNum, int maxStrLen);
        static int auxVectorSizeAndCount(AuxData *data, int *count, int *blockSize);
        static void buildAuxVector(AuxData *auxdata, ProcessControlBlock *process, userptr char *auxTable, userptr char *auxBlock);
        static int loadExecutable(const char *executablePath, AuxData *auxdata, void **entryPoint);
};

#endif
