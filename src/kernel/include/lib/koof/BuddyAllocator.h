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
 *   Name: BuddyAllocator.h                                                *
 *   Date: 25/11/2015                                                      *
 ***************************************************************************/

#ifndef _KOOF_BUDDYALLOCATOR_H_
#define _KOOF_BUDDYALLOCATOR_H_

class FreeChunk;

class BuddyAllocator
{
    public:
        ~BuddyAllocator();
        void init(int sizeInBlocks);

        int allocateBlocks(int sizeInBlocks);
        void freeBlocks(int firstBlockNumber, int sizeInBlocks);

     private:
        FreeChunk **m_lists;
        int m_orders;
};

#endif
