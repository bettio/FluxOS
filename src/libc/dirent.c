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
 *   Name: dirent.c                                                        *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/dirent.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
  FIXME: Quite HACKish implementation to work around some getdents limitations
         on kernel side.
*/

DIR *opendir(const char *name)
{
    int dirFD = open(name, O_RDONLY);
    if (dirFD < 0) {
        return NULL;
    }
    return fdopendir(dirFD);
}

DIR *fdopendir(int dirFD)
{
    struct DIR *newDir = (struct DIR *) malloc(sizeof(DIR));
    if (newDir) {
        newDir->dirFD = dirFD;
        newDir->entries = (struct dirent *) malloc(4096);
        if (!newDir->entries) {
            return NULL;
        }
        newDir->pos = 0;
        newDir->sizeInBytes = getdents(dirFD, newDir->entries, 4096);
    }
    return newDir;
}

int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    if (dirp->pos < dirp->sizeInBytes) {
        struct dirent *ent = dirp->entries;
        ent = (struct dirent *) (((unsigned long) ent) + dirp->pos);
        dirp->pos += ent->d_reclen;
        memcpy(entry, ent, ent->d_reclen);
        *result = entry;
    } else {
        *result = NULL;
    }

    return 0;
}

int closedir(DIR *dirp)
{
    int closeRet = close(dirp->dirFD);
    free(dirp->entries);
    free(dirp);

    return closeRet;
}
