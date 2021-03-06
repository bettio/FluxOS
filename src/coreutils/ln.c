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
 *   Name: ln.c                                                            *
 *   Date: 09/2010                                                         *
 ***************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{  
    if ((argc == 4) && (!strcmp(*++argv, "-s"))){
       const char *oldpath = *++argv;
       const char *newpath = *++argv;

       if (symlink(oldpath, newpath) < 0){                
            perror("ln");
   
            return EXIT_FAILURE;
       }

   }else if (argc == 3){
        const char *oldpath = *++argv;
        const char *newpath = *++argv;

        if (link(oldpath, newpath) < 0){
            perror("ln");
        
            return EXIT_FAILURE;
        }
     
    }else{
        printf("Wrong arguments\n");
        
        return EXIT_FAILURE;
    }
            
    return EXIT_SUCCESS;
}
