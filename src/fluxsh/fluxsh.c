/***************************************************************************
 *   Copyright 2011 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: fluxsh.c                                                        *
 ***************************************************************************/

#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/dirent.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/fluxos.h>
#include <sys/wait.h>

void cd_main(const char *c);
void seconds_main(void);
void shell_main(void);
void command_switch(const char *c);

char *cd;

int main(int argc, char *argv[])
{
	char *c = malloc(256);

	cd = malloc(256);

	while(1){
		c[0] = 0;

		getcwd(cd, 256);

		printf("FluxSH [%s] %c ", cd, (getuid() ? '$' : '#'));

		gets(c);

		command_switch(c);
	}
}

void command_switch(const char *c)
{
	if(!strcmp("", c)){

    }else if(!strncmp("echo", c, 4)){
        char *fName = c + 5;
        int len;
        while (*fName != '>') fName++;
        *fName = 0;

        int fd = creat(fName + 2, 0);
        write(fd, c + 5, strlen(c + 5));
        
	}else if(!strcmp("exit", c)){
		_exit(0);

	}else if(!strncmp("cd", c, 2)){
		cd_main(c);

	}else if(!strncmp("seconds", c, 7)){
		seconds_main();

	}else if(!strncmp("fork", c, 4)){
		int p = fork();

		waitpid(p, 0, 0);

	}else if(!strncmp("dogpf", c, 5)){
		//asm("cli");
	}else{
		char *myc = strdup(c);
		int i = 0;
		int z = strlen(myc);
		while((myc[i] != ' ') && (i < z)) i++;
		myc[i] = 0;
		char *myargs = (i < z) ? &myc[i] + 1 : "";
		char tmp[50];
		strcpy(tmp, "/bin/");
		strcat(tmp, myc);
		int status;

                int oe = open(tmp, 0, 0);
                close(oe);

		if (oe < 0){
			printf("FluxSH: %s: command not found\n", myc);
		}else{
            int status;
            int pid = fork();
            if (!pid){
                char *a[] = {tmp, myargs, 0};
                execve(tmp, a, 0);
            }

			waitpid(pid, &status, 0);
		}
	}
}


void cd_main(const char *c)
{
	char *c2 = (char *) c + 3;

	chdir(c2);
}

void seconds_main(void)
{
	printf("%i\n", time(0));
}
