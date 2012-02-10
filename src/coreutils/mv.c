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
 *   Name: mv.c                                                            *
 *   Date: 09/2010                                                         *
 ***************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv)
{
    const char *oldpath = *++argv;
    const char *newpath = *++argv;

    if (rename(oldpath, newpath) < 0){
        if (errno == EXDEV){
            int fdOld = open(oldpath, O_RDONLY);
            if (fdOld < 0){
                perror("mv");
                return EXIT_FAILURE;
            }
            
            int fdNew = creat(newpath, 666);
            if (fdNew < 0){
                perror("mv");
                close(fdOld);
                return EXIT_FAILURE;
            }
            
            char *tmpBuf = malloc(1024);
            if (tmpBuf == NULL){
                perror("mv");
                close(fdOld);
                close(fdNew);
                return EXIT_FAILURE;
            }
            int bufRead;
            
            while((bufRead = read(fdOld, tmpBuf, 1024))){
                write(fdNew, tmpBuf, bufRead);
            }

            if (unlink(oldpath) < 0){
                perror("mv");
                close(fdOld);
                close(fdNew);
                return EXIT_FAILURE;
            }

            close(fdOld);
            close(fdNew);
        }else{
            perror("mv");
        }
    }

    return EXIT_SUCCESS;
}
