/***************************************************************************
 *   Copyright 2016 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: unistd.c                                                        *
 ***************************************************************************/

#include <sys/utsname.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int gethostname(char *name, size_t len)
{
    struct utsname u;
    uname(&u);
    int nodelen = strlen(u.nodename);
    if (nodelen + 1 < len) {
        memcpy(name, u.nodename, nodelen + 1);
        return 0;
    } else {
        memcpy(name, u.nodename, len - 1);
        name[len - 1] = 0;
        errno = ENAMETOOLONG;
        return -1;
    }
}
