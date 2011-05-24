/***************************************************************************
 *   Copyright 2005 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: devid.h                                                         *
 *   Date: 30/08/2006                                                      *
 ***************************************************************************/

class DevId
{
    public:
        inline DevId()
        {
        }

        inline DevId(int major, int minor)
        {
            Major = major;
            Minor = minor;
        }
        
        int Major;
        int Minor;   
};

inline uint qHash(DevId id)
{
    return id.Minor + id.Major;
}

inline bool operator==(DevId id0, DevId id1)
{
    return (id0.Major == id1.Major) && (id0.Minor == id1.Minor);
}
