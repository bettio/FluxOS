/***************************************************************************
 *   Copyright 2010,2015 by Davide Bettio <davide.bettio@kdemail.net>      *
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
 *   Name: ls.c                                                            *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <sys/dirent.h>
#include <getopt.h>

typedef enum {
    NoFlags = 0,
    AllFlag = 1,
    InodeFlag = 2,
    ListFlag = 4,
    NumericFlag = 8
} LSFlags;

void printError(const char *fname);
void padString(char *out, int padSize);
void shortDate(char *dateStr, time_t date, time_t now);
int printFileInfo(const char *basePath, const char *name, LSFlags flags);

int main(int argc, char **argv)
{
    LSFlags flags = NoFlags;
    int opt;
    while ((opt = getopt(argc, argv, "ailn")) != -1) {
        switch (opt) {
            case 'a':
                flags |= AllFlag;
                break;
            case 'i':
                flags |= InodeFlag;
                break;
            case 'l':
                flags |= ListFlag;
                break;
            case 'n':
                flags |= NumericFlag;
                break;
            default:
                printf("Unrecognized argument\n");
                break;
        }
    }

    char pathBuf[256];
    char *path;
    if ((argc > 1) && (optind < argc)) {
        path = argv[optind];
    } else {
        getcwd(pathBuf, 256);
        path = pathBuf;
    }

    struct stat pathStat;
    lstat(path, &pathStat);
    if (!S_ISDIR(pathStat.st_mode)) {
        return (printFileInfo(path, "", flags) < 0) ? EXIT_FAILURE : EXIT_SUCCESS;
    }


    int readBytes;
    int pos;
    int dfd;
    struct dirent *ent = malloc(4096*15);

    dfd = open(path, 0, 0);
    if (dfd < 0){
        printError(path);
        return EXIT_FAILURE;
    }
    readBytes = getdents(dfd, ent, 4096*15);


    pos = 0;
    do{
        if (!(flags & AllFlag)) {
            if (ent->d_name[0] == '.') {
                pos += ent->d_reclen;
                ent = (struct dirent *) (((unsigned long) ent) + ent->d_reclen);
                continue;
            }
        }

        printFileInfo(path, ent->d_name, flags);

        pos += ent->d_reclen;
        ent = (struct dirent *) (((unsigned long) ent) + ent->d_reclen);
    }while(pos < readBytes);

    if (!(flags & ListFlag)) {
        printf("\n");
    }

    return EXIT_SUCCESS;
}

void printError(const char *fname)
{
    char *s = malloc(strlen("ls: ") + strlen(fname) + 1);
    strcpy(s, "ls: ");
    strcat(s, fname);

    perror(s);

    free(s);
}

void padString(char *out, int padSize)
{
    int outLen = strlen(out);
    if (outLen < padSize) {
        memmove(out + (padSize - outLen), out, outLen + 1);
        memset(out, ' ', padSize - outLen);
    }
}

void shortDate(char *dateStr, time_t date, time_t now)
{
    const char *months[] = {"jan", "feb", "mar", "apr", "may", "jun",
                           "jul", "aug", "sep", "oct", "nov", "dec"};

    struct tm tims;
    struct tm today;
    gmtime_r(&date, &tims);
    gmtime_r(&now, &today);

    strcpy(dateStr, months[tims.tm_mon]);
    strcat(dateStr, " ");
    if (((tims.tm_mon > today.tm_mon) && (tims.tm_year == today.tm_year - 1)) || (tims.tm_year == today.tm_year)) {
        /* print month and day */
        char day[3];
        uitoaz(tims.tm_mday, day, 10);
        strcat(dateStr, day);
    } else {
        /* print month and year */
        char year[5];
        uitoaz(tims.tm_year + 1900, year, 10);
        strcat(dateStr, year);
    }
}

int printFileInfo(const char *basePath, const char *name, LSFlags flags)
{
    char *tmpPath = malloc(strlen(name) + strlen(basePath) + 2);
    strcpy(tmpPath, basePath);
    if (strlen(name)) {
        strcat(tmpPath, "/");
        strcat(tmpPath, name);
    } else {
        name = basePath;
    }

    struct stat s;
    int statRes = lstat(tmpPath, &s);
    if (statRes < 0) {
        printError(tmpPath);
        free(tmpPath);
        return statRes;
    }

    free(tmpPath);

    char inodeNum[32];
    if (flags & InodeFlag) {
        uitoaz(s.st_ino, inodeNum, 10);
        padString(inodeNum, 8);
        strcat(inodeNum, " ");
    } else {
        inodeNum[0] = 0;
    }

    char modestr[11] = "----------";

    if (flags & ListFlag) {
        if (S_ISLNK(s.st_mode)){
            modestr[0] = 'l';
        }else if(S_ISREG(s.st_mode)){
            modestr[0] = '-';
        }else if(S_ISDIR(s.st_mode)){
            modestr[0] = 'd';
        }else if(S_ISCHR(s.st_mode)){
            modestr[0] = 'c';
        }else if(S_ISBLK(s.st_mode)){
            modestr[0] = 'b';
        }else if(S_ISFIFO(s.st_mode)){
            modestr[0] = '?';
        }else if(S_ISSOCK(s.st_mode)){
            modestr[0] = '.';
        }

        if (s.st_mode & S_IRUSR) modestr[1] = 'r';
        if (s.st_mode & S_IWUSR) modestr[2] = 'w';
        if (s.st_mode & S_IXUSR) modestr[3] = 'x';

        if (s.st_mode & S_IRGRP) modestr[4] = 'r';
        if (s.st_mode & S_IWGRP) modestr[5] = 'w';
        if (s.st_mode & S_IXGRP) modestr[6] = 'x';

        if (s.st_mode & S_IROTH) modestr[7] = 'r';
        if (s.st_mode & S_IWOTH) modestr[8] = 'w';
        if (s.st_mode & S_IXOTH) modestr[9] = 'x';

        char nlink[10];
        uitoaz(s.st_nlink, nlink, 10);
        padString(nlink, 3);

        char username[32];
        char groupname[32];

        if (!(flags & NumericFlag)) {
            struct passwd *result = NULL;
            struct group *gresult = NULL;

            struct passwd pw;
            char buf[512];
            getpwuid_r(s.st_uid, &pw, buf, 512, &result);

            if (result != NULL) {
                strncpy(username, result->pw_name, 32);
            } else {
                uitoaz(s.st_uid, username, 10);
            }

            struct group gr;
            char grbuf[512];
            getgrgid_r(s.st_gid, &gr, grbuf, 512, &gresult);

            if (gresult != NULL) {
                strncpy(groupname, gresult->gr_name, 32);
            } else {
                uitoaz(s.st_gid, groupname, 10);
            }
        } else { 
            uitoaz(s.st_uid, username, 10);
            uitoaz(s.st_gid, groupname, 10);
        }
        padString(username, 8);
        padString(groupname, 8);

        char date[9];
        shortDate(date, (time_t) s.st_mtime, time(NULL));
        padString(date, 8);

        char fsize[10];
        uitoaz(s.st_size, fsize, 10);
        padString(fsize, 8);

        printf("%s%s %s %s %s %s %s %s\n", inodeNum, modestr, nlink, username, groupname, fsize, date, name);

    } else {
        printf("%s ", name);
    }

    return 0;
}
