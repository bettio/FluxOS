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
 *   Name: cpuregistersframe.h                                             *
 *   Date: 14/11/2015                                                      *
 ***************************************************************************/

#ifndef CPUREGISTERSFRAME_H_
#define CPUREGISTERSFRAME_H_

#ifndef NO_C_CODE

struct RegistersFrame
{
    /* $r1 .. $r32 */
    unsigned long registers[31];
    unsigned long pc;
};

#endif

#define CPU_REGISTERS_FRAME_SIZE -128

#define REGISTERS_FRAME_GP_REGISTER 27
#define REGISTERS_FRAME_SP_REGISTER 28
#define REGISTERS_FRAME_RA_REGISTER 30

#endif
