/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: ls.c                                                            *
 ***************************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/dirent.h>
/*
#include <linux/unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
*/


/*TODO: we must fix ls for a single file*/
/*FIXME: ugly ugly ugly code */
int main(int argc, char **argv)
{
    int readBytes;
    int pos;
    struct dirent *ent = malloc(4096*15);
    struct stat *s = malloc(sizeof(struct stat));    

    int procDir = open("/proc", O_RDONLY, 0);
    readBytes = getdents(procDir, ent, 4096*15);


    pos = 0;

    do{
        char buf[64];
        sprintf(buf, "/proc/%s", ent->d_name); 
        lstat(buf, s);

        if(S_ISDIR(s->st_mode) && isdigit(ent->d_name[0])){
            printf("%s\n", ent->d_name);
        }

        pos += ent->d_reclen;
        ent = (struct dirent *) (((unsigned long) ent) + ent->d_reclen);
    }while(pos < readBytes);

    return EXIT_SUCCESS;
}
