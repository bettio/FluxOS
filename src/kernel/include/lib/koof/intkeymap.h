/***************************************************************************
 *   Copyright 2007 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: intkeymap.h                                                     *
 *   Date: 21/01/2007                                                      *
 ***************************************************************************/

#ifndef _LIB_KOOF_INTKEYMAP_H_
#define _LIB_KOOF_INTKEYMAP_H_

#include <core/printk.h>
#include <cstdlib.h>
#include <new>

#warning DEPRECATED DEPRECATED!!

template <class ItemType> class IntKeyMap{
	public:
		IntKeyMap()
		{
			MapSize = 0;

			Root.FirstItem = 0;
			Root.LastItem = 0;
		}

		int Add(int key, ItemType i)
		{
			KeyMapItem *item = new KeyMapItem(key, i);

			if (MapSize == 0){
				Root.FirstItem = item;
				Root.LastItem = item;
			}else{
				KeyMapItem *tmpitem = Root.FirstItem;
		
				while(true){
					//printk("Item attuale: %s\n", tmpitem->Key);
					if (tmpitem->Key == key) return -1;

					if (tmpitem->NextItem == 0){
						tmpitem->NextItem = item;
						item->PrevItem = tmpitem;

						break;
					}else{
						tmpitem = tmpitem->NextItem;
					}
				}
			}

			MapSize++;

			return 0;
		}

		void Remove(int key)
		{
			KeyMapItem *tmpitem = Root.FirstItem;

			while(true){
				if (tmpitem->Key == key){
					tmpitem->PrevItem->NextItem = tmpitem->NextItem;
					
					MapSize--;

					return;
				}else if (tmpitem->NextItem != 0){
					tmpitem = tmpitem->NextItem;

				}else{
					return;
				}
			}
		}

		void Clear()
		{
			Root.FirstItem = 0;
			Root.LastItem = 0;
		}

		int Size()
		{
			return MapSize;
		}

		bool IsEmpty()
		{
			return (MapSize == 0);
		}

		ItemType Item(int key)
		{
			KeyMapItem *tmpitem = Root.FirstItem;

			//printk("Ricerco un elemento: %s\n", key);

			while(true){
				//printk("Elemento attuale: %s", tmpitem->Key);

				if (tmpitem->Key == key){
					return tmpitem->ItemPtr;
				}

				if (tmpitem->NextItem != 0){
					tmpitem = tmpitem->NextItem;
				}else{
					///printk("NON HO TROVATO NESSUN ELEMENTO: %i!\n", key);
					return 0;
				}
			}
		}

        int ItemKeyByIndex(int index)
        {
            KeyMapItem *tmpitem = Root.FirstItem;
            
            for (int i = 1; i < index; i++){
                tmpitem = tmpitem->NextItem;
            }
            
            return tmpitem->Key;
        }

        ItemType ItemByIndex(int index)
        {
            KeyMapItem *tmpitem = Root.FirstItem;
            
            for (int i = 1; i < index; i++){
                tmpitem = tmpitem->NextItem;
            }
            
            return tmpitem->ItemPtr;
        }

		ItemType operator[](int key)
		{
			return Item(key);
		}

	private:
		class KeyMapItem
		{
			public:
				KeyMapItem(int key, ItemType item)
				{
					PrevItem = 0;
					NextItem = 0;

					Key = key;
					ItemPtr = item;
				}

				KeyMapItem *PrevItem;
				KeyMapItem *NextItem;
				int Key;
				ItemType ItemPtr;
		};

		class KeyMapRootItem
		{
			public:
				KeyMapRootItem()
				{
					FirstItem = 0;
					LastItem = 0;
				}

				KeyMapItem *FirstItem;
				KeyMapItem *LastItem;
		};

		int MapSize;
		KeyMapRootItem Root;
};

#endif
