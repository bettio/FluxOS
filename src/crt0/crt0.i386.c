/*
Copyright (C) 2011 Davide Bettio <davide.bettio@kdemail.net>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <unistd.h>

extern int main(int argc, char **argv, char **envp);

asm(
".globl _start\n"
"_start:\n"
"popl %ecx\n" /* argc */
"movl %esp,%esi\n" /* argv */
"leal 4(%esi,%ecx,4),%eax\n" /* envp */
"pushl %eax\n"
"pushl %esi\n"
"pushl %ecx\n"
"call initmem\n"
"call main\n"
"push %eax\n"
"call _exit\n"
);

