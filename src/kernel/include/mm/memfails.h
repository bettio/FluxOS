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
 *   Name: memfails.h                                                      *
 *   Date: 29/05/2011                                                      *
 ***************************************************************************/

#ifndef _MM_MEMFAILS_H_
#define _MM_MEMFAILS_H_

#if __cplusplus
extern "C"
{
#endif
    void mallocAbort();
    void mallocFail();
    void mallocUsageError(void *ptr);
    void mallocCorruptionError(void *ptr);

#if __cplusplus
}
#endif

#endif
