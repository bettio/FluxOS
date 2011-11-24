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
 *   Name: cat.c                                                           *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void printError(char *fname)
{
    /* strlen("cat: ") = 5 */
    char *s = malloc(5 + strlen(fname) + 1);
    strcpy(s, "cat: ");
    strcat(s, fname);

    perror(s);

    free(s);
}

int main(int argc, char **argv)
{
    int exitVal = 0;

    while(*++argv){
        char c;
        int readRetVal;
        int tfd = open(*argv, 0, 0);

        if (tfd < 0){
            printError(*argv);

            exitVal++;
        }else{
            while ((readRetVal = read(tfd, &c, 1)) > 0){
                write(STDOUT_FILENO, &c, 1); /*We must check write return value*/
            }

            if (readRetVal < 0){
                printError(*argv);

                exitVal++;
            }

            if (close(tfd) < 0){
                printError(*argv);

                exitVal++;
            }
        
        }
    }

    return (exitVal > 0);
}
