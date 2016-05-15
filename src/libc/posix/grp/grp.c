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
 *   Name: grp.c                                                           *
 ***************************************************************************/

#include <grp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static int parseGroupLine(char *line, struct group *gr_buf)
{
    char *fPtr = line;
    char *prevPtr = line;
    int fieldN = 0;
    while ((*fPtr != '\n') && (*fPtr != 0)) {
        switch (fieldN) {
            case 0:
                gr_buf->gr_name = prevPtr;
                break;
            case 1:
                gr_buf->gr_passwd = prevPtr;
                break;
            case 2:
                gr_buf->gr_gid = atoi(prevPtr);
                break;
            case 3:
                gr_buf->gr_mem = 0;
                break;
            default:
                return -1;
                break;
        }
        fPtr++;
        if (*fPtr == ':') {
            fieldN++;
            *fPtr = 0;
            fPtr++;
            prevPtr = fPtr;
        }
    }
    return 0;
}

int getgrnam_r(const char *name, struct group *grp, char *buf, size_t buflen, struct group **result)
{
    FILE *groupFile = fopen("/etc/group", "r");
    if (!groupFile) {
        *result = NULL;
        return -1;
    }

    char *line = buf;

    while (1) {
        if (!fgets(line, buflen, groupFile)) {
            break;
        }
        parseGroupLine(line, grp);
        if (!strcmp(grp->gr_name, name)) {
            *result = grp;
            fclose(groupFile);
            return 0;
        }
    }

    *result = NULL;
    fclose(groupFile);
    return -1;
}

int getgrgid_r(gid_t gid, struct group *grp, char *buf, size_t buflen, struct group **result)
{
    FILE *groupFile = fopen("/etc/group", "r");
    if (!groupFile) {
        *result = NULL;
        return -1;
    }

    char *line = buf;

    while (1) {
        if (!fgets(line, buflen, groupFile)) {
            break;
        }
        parseGroupLine(line, grp);
        if (grp->gr_gid == gid) {
            *result = grp;
            fclose(groupFile);
            return 0;
        }
    }

    *result = NULL;
    fclose(groupFile);
    return -1;
}
