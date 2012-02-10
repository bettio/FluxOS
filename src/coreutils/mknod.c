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
 *   Name: mknod.c                                                         *
 *   Date: 09/2010                                                         *
 ***************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

extern int mknod(const char *pathname, mode_t mode, dev_t dev); //TODO: Remove me

int main(int argc, char **argv)
{
    if (argc == 5){
        const char *name = *++argv;
        const char *type = *++argv;
        int major = atoi(*++argv);
        int minor = atoi(*++argv);
        int t = 0;
        
        if (!strcmp(type, "c")){
            t = S_IFCHR;
        }else if (!strcmp(type, "b")){
            t = S_IFBLK;
        }

        if (mknod(name, t, major << 8 | minor) < 0){
            perror("mknod");
        }
   }else{
       printf("Wrong arguments number\n");
   }
}
