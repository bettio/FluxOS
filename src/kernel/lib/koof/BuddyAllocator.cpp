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
 *   Name: BuddyAllocator.cpp                                              *
 *   Date: 25/11/2015                                                      *
 ***************************************************************************/

#include <BuddyAllocator.h>

//#include <core/printk.h>
#include <cstdlib.h>

class FreeChunk
{
    public:
        FreeChunk *prev;
        FreeChunk *next;
        int blockIndex;
};

BuddyAllocator::~BuddyAllocator()
{
}

inline int log2i(int n, int maxLog)
{
    for (int i = maxLog; i >= 0; i--) {
        if (n & (1 << i)) {
            return i;
        }
    }

    return -1;
}

void BuddyAllocator::init(int blocks)
{

    m_orders = log2i(blocks, 32) + 1;
    m_lists = new FreeChunk *[m_orders];

    for (int i = 0; i < m_orders; i++) {
        m_lists[i] = NULL;
    }

    FreeChunk *fc = new FreeChunk;
    fc->prev = NULL;
    fc->next = NULL;
    fc->blockIndex = 0;
    m_lists[m_orders - 1] = fc;
}

int BuddyAllocator::allocateBlocks(int sizeInBlocks)
{
    int index = log2i(sizeInBlocks, m_orders);
    index += ((sizeInBlocks & ~(1 << index)) != 0);
    int foundBlockIndex;

//    printk("m_order: %i\n", index);
    for (int i = index; i < m_orders; i++) {
        if (m_lists[i]) {
//            printk("m_lists[%i]: %p\n", i, m_lists[i]);
            FreeChunk *fc = m_lists[i];
            m_lists[i] = fc->next;
            foundBlockIndex = fc->blockIndex;
            delete fc;

            int remainingFreeBlocks = (1 << i) - sizeInBlocks;

//            printk("remainingFreeBlocks: %x\n", remainingFreeBlocks);

            for (int j = i - 1; j >= 0; j--) {
                 if (remainingFreeBlocks & (1 << j)) {
                     FreeChunk *nF = new FreeChunk;
                     if (m_lists[j]) {
                         m_lists[j]->prev = nF;
                     }
                     nF->blockIndex = foundBlockIndex + sizeInBlocks + remainingFreeBlocks - (1 << j);
                     remainingFreeBlocks &= ~(1 << j);
  //                   printk("i: %i s: %i j: %i , ", nF->blockIndex, (1 << j), j);
                     nF->next = m_lists[j];
                     m_lists[j] = nF;
                 }
            }

            return foundBlockIndex;
        }
    }
    return -1;
}

void BuddyAllocator::freeBlocks(int firstBlockNumber, int sizeInBlocks)
{

}
