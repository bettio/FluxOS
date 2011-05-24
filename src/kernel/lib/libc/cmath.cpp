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
 *   Name: cmath.cpp                                                       *
 *   Date: 22/05/2011                                                      *
 ***************************************************************************/

extern "C"
{
    int intPolynomialEval(int x, int *coefficents, int coefficentsLen)
    {
        int val = 0;
        int tmpX = 1;

        for (int i = 0; i < coefficentsLen; i++){
            tmpX *= x;
            val += coefficents[i] * tmpX;
        }
        
        return val;
    }

    unsigned int uintpow(unsigned int n, unsigned int m)
    {
        unsigned int val = 1;
        unsigned int i = m;

        while (i > 0){
            val *= n;
            i--;
        }

        return val;
    }
}
