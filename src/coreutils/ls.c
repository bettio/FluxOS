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
    int dfd;
    char *cd;
    char *tmpPath;
    struct dirent *ent = malloc(4096*15);
    struct stat *s = malloc(sizeof(struct stat));    

    if (argc == 2){
        cd = *++argv;
    }else{
        cd = malloc(256);
        getcwd(cd, 256);
    }

    dfd = open(cd, 0, 0); /*verificare questa open*/
    readBytes = getdents(dfd, ent, 4096*15);


    pos = 0;
    /*int j = 0;*/

    do{
        /*j++;*/
        char modestr[11] = "----------";

        tmpPath = malloc(strlen(ent->d_name) + strlen(cd) + 2);
        strcpy(tmpPath, cd);
        strcat(tmpPath, "/");
        strcat(tmpPath, ent->d_name);


        lstat(tmpPath, s);

        free(tmpPath);

        if (S_ISLNK(s->st_mode)){
            modestr[0] = 'l';
        }else if(S_ISREG(s->st_mode)){
            modestr[0] = '-';
        }else if(S_ISDIR(s->st_mode)){
            modestr[0] = 'd';
        }else if(S_ISCHR(s->st_mode)){
            modestr[0] = 'c';
        }else if(S_ISBLK(s->st_mode)){
            modestr[0] = 'b';
        }else if(S_ISFIFO(s->st_mode)){
            modestr[0] = '?';
        }else if(S_ISSOCK(s->st_mode)){
            modestr[0] = '.';
        }

        if (s->st_mode & S_IRUSR) modestr[1] = 'r';
        if (s->st_mode & S_IWUSR) modestr[2] = 'w';
        if (s->st_mode & S_IXUSR) modestr[3] = 'x';

        if (s->st_mode & S_IRGRP) modestr[4] = 'r';
        if (s->st_mode & S_IWGRP) modestr[5] = 'w';
        if (s->st_mode & S_IXGRP) modestr[6] = 'x';

        if (s->st_mode & S_IROTH) modestr[7] = 'r';
        if (s->st_mode & S_IWOTH) modestr[8] = 'w';
        if (s->st_mode & S_IXOTH) modestr[9] = 'x';

        /*printf("%s  %i %i %i %i\t%i\t%s\n", modestr, s->st_nlink, s->st_uid, s->st_gid, s->st_size, s->st_mtime, ent->d_name);*/
        printf("%s  %s\n", modestr, ent->d_name);

        pos += ent->d_reclen;

        ent = (struct dirent *) (((unsigned long) ent) + ent->d_reclen);
    }while(pos < readBytes);

    return EXIT_SUCCESS;
}
