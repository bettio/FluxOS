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
 *   Name: syscallsnr.h                                                    *
 *   Date: 14/11/2015                                                      *
 ***************************************************************************/

#ifndef _SYSCALLS_NR_
#define _SYSCALLS_NR_

#define __NR_SYSCALLS   4000
#define __NR_EXIT	4001
#define __NR_FORK	4002
#define __NR_READ	4003
#define __NR_WRITE	4004
#define __NR_OPEN	4005
#define __NR_CLOSE	4006
#define __NR_WAITPID    4007
#define __NR_CREAT	4008
#define __NR_LINK       4009
#define __NR_UNLINK	4010
#define __NR_EXECVE	4011
#define __NR_CHDIR	4012
#define __NR_TIME	4013
#define __NR_MKNOD      4014
#define __NR_CHMOD      4015
#define __NR_LCHOWN     4016
#define __NR_BREAK      4017
/* unused */
#define __NR_LSEEK      4019
#define __NR_GETPID     4020
#define __NR_MOUNT      4021
#define __NR_UMOUNT     4022
#define __NR_GETUID	4024
#define __NR_RENAME	2038
#define __NR_MKDIR	4039
#define __NR_RMDIR	4040
#define __NR_BRK        4045
#define __NR_UMOUNT2    4052
#define __NR_IOCTL	4054
#define __NR_SETHOSTNAME 4074
#define __NR_SYMLINK    4083
#define __NR_READLINK	4085
#define __NR_REBOOT	4088
#define __NR_FCHMOD	4094
#define __NR_FCHOWN     4095
#define __NR_LSTAT	4107
#define __NR_SETDOMAINNAME 4121
#define __NR_UNAME      4122
#define __NR_GETDENTS	4141
#define __NR_SOCKET     4183
#define __NR_CHOWN	4202
#define __NR_GETCWD	4203

#endif
