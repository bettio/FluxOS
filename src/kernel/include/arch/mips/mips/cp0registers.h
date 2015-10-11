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
 *   Name: cp0registers.h                                                  *
 *   Date: 14/11/2015                                                      *
 ***************************************************************************/



#ifndef _MIPS_CP0REGISTERS_
#define _MIPS_CP0REGISTERS_

#define CP0_Index_Register $0
#define CP0_Random_Register $1
#define CP0_EntryLo0_Register $2
#define CP0_EntryLo1_Register $3
#define CP0_PageMask_Register $5
#define CP0_EntryHi_Register $10

#define CP0_BadVAddr_Register $8
#define CP0_Status_Register $12
#define CP0_Cause_Register $13
#define CP0_EPC_Register $14

#define CP0_EBase_Register $15
#define CP0_EBase_Select 1

#define STATUS_REGISTER_EXL_SET 0x1

#endif
