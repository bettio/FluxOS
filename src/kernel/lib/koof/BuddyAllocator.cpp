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

#include <core/printk.h>
#include <cstdlib.h>

#define ENABLE_DEBUG_MSG 0
#include <debugmacros.h>

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

    for (int i = index; i < m_orders; i++) {
        if (m_lists[i]) {
            FreeChunk *fc = m_lists[i];
            // we just take the first one from the linked list
            m_lists[i] = fc->next;
            foundBlockIndex = fc->blockIndex;
            delete fc;

            int remainingFreeBlocks = (1 << i) - sizeInBlocks;

            DEBUG_MSG("BuddyAllocator::allocateBlocks(%i): ", sizeInBlocks);
            for (int j = i - 1; j >= 0; j--) {
                 if (remainingFreeBlocks & (1 << j)) {
                     FreeChunk *nF = new FreeChunk;
                     if (m_lists[j]) {
                         m_lists[j]->prev = nF;
                     }
                     nF->blockIndex = foundBlockIndex + sizeInBlocks + remainingFreeBlocks - (1 << j);
                     DEBUG_MSG(" i: %i, s: %i. ", nF->blockIndex, (1 << j));
                     remainingFreeBlocks &= ~(1 << j);
                     nF->next = m_lists[j];
                     m_lists[j] = nF;
                 }
            }
            DEBUG_MSG("\n");
            return foundBlockIndex;
        }
    }
    return -1;
}

int BuddyAllocator::allocateAlignedBlocks(int blockIndex)
{
    for (int i = m_orders - 1; i >= 0; i--) {
        if (m_lists[i]) {
            FreeChunk *fc = m_lists[i];
            do {
                if (fc->blockIndex == blockIndex) {
                    if (fc->prev) {
                        fc->prev->next = fc->next;
                    } else {
                        m_lists[i] = fc->next;
                    }
                    fc->next->prev = fc->prev;
                    delete fc;

                    return 1 << i;
                }

                fc = fc->next;
            } while (fc);
        }
    }
    return 0;
}

int BuddyAllocator::allocateBlocks(int blockIndex, int sizeInBlocks)
{
    //I have no better idea than iterating all the chunks
    //let's start from bigger blocks, I really hope we'll find the right
    //free space block there before iterating a huge list of small chunks
    for (int i = m_orders - 1; i >= 0; i--) {
        if (m_lists[i]) {
            FreeChunk *fc = m_lists[i];
            do {
                int chunkSizeInBlocks = (1 << i);
                if ((fc->blockIndex <= blockIndex) && (blockIndex <= fc->blockIndex + chunkSizeInBlocks)) {
                    if (fc->prev) {
                        fc->prev->next = fc->next;
                    } else {
                        m_lists[i] = fc->next;
                    }
                    if (fc->next) {
                        fc->next->prev = fc->prev;
                    }
                    int remainingFreeBlocks = blockIndex - fc->blockIndex;
                    int newBlockIndex = blockIndex - remainingFreeBlocks;

                    DEBUG_MSG("BuddyAllocator::allocateBlocks(%i, %i): ", blockIndex, sizeInBlocks);
                    for (int j = i - 1; j >= 0; j--) {
                        if (remainingFreeBlocks & (1 << j)) {
                            FreeChunk *nF = new FreeChunk;
                            if (m_lists[j]) {
                                m_lists[j]->prev = nF;
                            }
                            DEBUG_MSG(" i: %i, s: %i. ", nF->blockIndex, (1 << j));
                            nF->blockIndex = newBlockIndex;
                            newBlockIndex += (1 << j);
                            nF->next = m_lists[j];
                            m_lists[j] = nF;
                        }
                    }
                    DEBUG_MSG("\n");

                    int blocksToRead = -(chunkSizeInBlocks - sizeInBlocks - (blockIndex - fc->blockIndex));
                    int lastReadChunkSize = chunkSizeInBlocks;
                    int remainingSize = 0;
                    if (blocksToRead > 0) {
                        newBlockIndex += fc->blockIndex + chunkSizeInBlocks - blockIndex;
                        do {
                            lastReadChunkSize = allocateAlignedBlocks(newBlockIndex);
                            DEBUG_MSG(" i2: %i, s2: %i. ", newBlockIndex, lastReadChunkSize);
                            blocksToRead -= lastReadChunkSize;
                            newBlockIndex += lastReadChunkSize;
                        } while (blocksToRead > 0);

                        newBlockIndex -= lastReadChunkSize;
                        remainingSize = blockIndex + sizeInBlocks - newBlockIndex;
                    } else {
                        newBlockIndex += sizeInBlocks;
                    }


                    if (blocksToRead < 0) {
                        remainingFreeBlocks = -blocksToRead;
                        for (int j = i - 1; j >= 0; j--) {
                            if (remainingFreeBlocks & (1 << j)) {
                                FreeChunk *nF = new FreeChunk;
                                if (m_lists[j]) {
                                    m_lists[j]->prev = nF;
                                }
                                nF->blockIndex = newBlockIndex + remainingSize + remainingFreeBlocks - (1 << j);
                                DEBUG_MSG(" i3: %i, s3: %i. ", nF->blockIndex, (1 << j));
                                remainingFreeBlocks &= ~(1 << j);
                                nF->next = m_lists[j];
                                m_lists[j] = nF;
                            }
                        }
                        DEBUG_MSG("\n");
                    }
                    delete fc;
                    return blockIndex;
                }
                fc = fc->next;
            } while (fc);
        }
    }
    return -1;
}

int BuddyAllocator::freeAlignedBlock(int blockIndex, int order)
{
    FreeChunk *fc = new FreeChunk;
    fc->prev = NULL;
    fc->next = m_lists[order];
    fc->blockIndex = blockIndex;
    if (m_lists[order]) {
        m_lists[order]->prev = fc;
    }
    m_lists[order] = fc;
}

void BuddyAllocator::freeBlocks(int firstBlockNumber, int sizeInBlocks)
{
    int pos = firstBlockNumber;
    int tmp = 0;
    for (int i = 0; i < sizeof(int) * 8; i++) {
        if (pos & (1 << i)) {
            if (pos + (1 << i) <= firstBlockNumber + sizeInBlocks) {
               DEBUG_MSG("r: %i s: %i. ", pos, 1 << i);
               freeAlignedBlock(pos, 1 << i);
               pos += 1 << i;
            }
        }
    }

    int remaining = (firstBlockNumber + sizeInBlocks) - pos;
    DEBUG_MSG("pos: %i, diff: %i\n", pos, remaining);
    for (int i = sizeof(int) * 8; i >= 0; i--) {
        if (remaining & (1 << i)) {
            freeAlignedBlock(pos, 1 << i);
            pos += (1 << i);
            DEBUG_MSG("r2: %i, s2: %i. ", pos, 1 << i);
        }
    }
}
