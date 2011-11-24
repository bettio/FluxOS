/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: ctype.c                                                         *
 *   Date: 01/02/2006                                                      *
 ***************************************************************************/

#include <ctype.h>

int isalnum(int ch)
{
    return (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')));
}

int isalpha(int ch)
{
    return (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')));
}

int iscntrl(int ch)
{
    return (((ch >= 0) && (ch <= 31)) || (ch == 127));
}

int isdigit(int ch)
{
    return ((ch >= '0') && (ch <= '9'));
}

int isgraph(int ch)
{
    return (((ch >= 33) && (ch <= 126)));
}

int islower(int ch)
{
    return ((ch >= 'a') && (ch <= 'z'));
}

int isprint(int ch)
{
    return (((ch >= 32) && (ch <= 126)));
}

int ispunct(int ch)
{
    return (((ch >= 33) && (ch <= 47)) || ((ch >= 58) && (ch <= 64)) ||
            ((ch >= 91) && (ch <= 96)) || ((ch >= 123) && (ch <= 126)));
}

int isspace(int ch)
{
    return (((ch >= 9 ) && (ch <= 13)) || (ch == 32));
}

int isupper(int ch)
{
    return ((ch >= 'A') && (ch <= 'Z'));
}

int isxdigit(int ch)
{
    return (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')));
}

int tolower(int ch)
{
    return ch + (((ch > 64) && (ch < 91)) ? 32 : 0);
}

int toupper(int ch)
{
    return ch - (((ch > 96) && (ch < 123)) ? 32 : 0);
}
