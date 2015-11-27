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
 *   Name: mmap.c                                                          *
 ***************************************************************************/

#define _GNU_SOURCE

#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[], char *envp[])
{
    int i;
    int exitStatus = EXIT_SUCCESS;
    unsigned long *mappedA;

    printf("test1: ");
    if ((mmap(NULL, 4096*10, PROT_READ, MAP_ANONYMOUS, -1, 0) != ((void *) -1)) && (errno != EINVAL)) {
        printf("ERR: %i\n", errno);
        exitStatus = EXIT_FAILURE;
    } else {
        printf("OK\n");
    }

    printf("test2: ");
    if ((mmap(NULL, 4096*10, PROT_READ, MAP_ANONYMOUS | MAP_SHARED | MAP_PRIVATE, -1, 0) != ((void *) -1)) && (errno != EINVAL)) {
        printf("ERR: %i\n", errno);
        exitStatus = EXIT_FAILURE;
    } else {
        printf("OK\n");
    }

    printf("test3: ");
    mappedA = mmap(NULL, 4096*15, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (mappedA == (unsigned long *) -1) {
        printf("ERR: %i\n", errno);
        exitStatus = EXIT_FAILURE;
    } else {
        printf("OK\n");
    }

   printf("test4: ");
   for (i = 0; i < 4096*15 / sizeof(unsigned long); i++) {
      mappedA[i] = i;
   }
   for (i = 0; i < 4096*15 / sizeof(unsigned long); i++) {
       if (mappedA[i] != i) {
           printf("ERR: %i\n", (int) mappedA[i]);
           exitStatus = EXIT_FAILURE;
       }
   }
   printf("OK\n");

   printf("test5: ");
   if (munmap(mappedA, 4096*15) != 0) {
       printf("ERR: %i\n", errno);
       exitStatus = EXIT_FAILURE;
   } else {
       printf("OK\n");
   }

   printf("test6: ");
   mappedA = mmap(NULL, 4096*15, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
   if (mappedA == (unsigned long *) -1) {
       printf("ERR: %i\n", errno);
       exitStatus = EXIT_FAILURE;
   } else {
        printf("OK\n");
    }

   printf("test7: ");
   for (i = 0; i < 4096*15 / sizeof(unsigned long); i++) {
       if (mappedA[i] != 0) {
           printf("ERR: %i\n", (int) mappedA[i]);
           exitStatus = EXIT_FAILURE;
       }
   }
   printf("OK\n");

   printf("test8: ");
   if (munmap(mappedA - 2048, 4096*20) != 0) {
       printf("ERR: %i\n", errno);
       exitStatus = EXIT_FAILURE;
   } else {
       printf("OK\n");
   }

#ifdef TEST_WRITE_ON_UNMAPPED
   mappedA[10] = 25;
   printf("Error: it didn't segfault\n");
#endif

   return exitStatus;
}

