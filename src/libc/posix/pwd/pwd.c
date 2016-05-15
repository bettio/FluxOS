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
 *   Name: pwd.c                                                           *
 ***************************************************************************/

#include <pwd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static int parsePasswdLine(char *line, struct passwd *pw_buf)
{
    char *fPtr = line;
    char *prevPtr = line;
    int fieldN = 0;
    while ((*fPtr != '\n') && (*fPtr != 0)) {
        switch (fieldN) {
            case 0:
                pw_buf->pw_name = prevPtr;
                break;
            case 1:
                pw_buf->pw_passwd = prevPtr;
                break;
            case 2:
                pw_buf->pw_uid = atoi(prevPtr);
                break;
            case 3:
                pw_buf->pw_gid = atoi(prevPtr);
                break;
            case 4:
                pw_buf->pw_gecos = prevPtr;
                break;
            case 5:
                pw_buf->pw_dir = prevPtr;
                break;
            case 6:
                pw_buf->pw_shell = prevPtr;
                break;
            case 7:
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

int getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
    FILE *passwdFile = fopen("/etc/passwd", "r");
    if (!passwdFile) {
        *result = NULL;
        return -1;
    }

    char *line = buf;

    while (1) {
        if (!fgets(line, buflen, passwdFile)) {
            break;
        }
        parsePasswdLine(line, pwd);
        if (!strcmp(pwd->pw_name, name)) {
            *result = pwd;
            fclose(passwdFile);
            return 0;
        }
    }

    *result = NULL;
    fclose(passwdFile);
    return -1;
}

int getpwuid_r(uid_t uid, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
    FILE *passwdFile = fopen("/etc/passwd", "r");
    if (!passwdFile) {
        *result = NULL;
        return -1;
    }

    char *line = buf;

    while (1) {
        if (!fgets(line, buflen, passwdFile)) {
            break;
        }
        parsePasswdLine(line, pwd);
        if (pwd->pw_uid == uid) {
            *result = pwd;
            fclose(passwdFile);
            return 0;
        }
    }

    *result = NULL;
    fclose(passwdFile);
    return -1;
}
