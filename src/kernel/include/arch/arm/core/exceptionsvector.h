/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: exceptionsvector.h                                              *
 *   Date: 29/09/2010                                                      *
 ***************************************************************************/

#ifndef _EXCEPTIONSVECTOR_H_
#define _EXCEPTIONSVECTOR_H_

class ExceptionsVector
{
    public:
        enum ExceptionType
        {
            SWI = 2,
            DataAbort = 4,
            IRQ = 6,
            FIQ = 7
        };
        
        static void init();
        static void setHandler(ExceptionType eTye, void *handler);
        static void *handler(ExceptionType eType);
};

#endif
