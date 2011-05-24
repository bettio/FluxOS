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
 *   Name: keymap.h                                                        *
 *   Date: 21/01/2007                                                      *
 ***************************************************************************/

#ifndef _KOOF_KEYMAP_H_
#define _KOOF_KEYMAP_H_

#include <core/printk.h>
#include <cstring.h>
#include <cstdlib.h>
#include <new>

#warning DEPRECATED DEPRECATED!!

template <class ItemType> class KeyMap{
	public:
		KeyMap()
		{
			MapSize = 0;

			Root.FirstItem = 0;
			Root.LastItem = 0;
		}

		int Add(const char *key, ItemType i)
		{
			KeyMapItem *item = new KeyMapItem(key, i);

			if (MapSize == 0){
				Root.FirstItem = item;
				Root.LastItem = item;
			}else{
				KeyMapItem *tmpitem = Root.FirstItem;
		
				while(true){
					//printk("Item attuale: %s\n", tmpitem->Key);
					if (!strcmp(tmpitem->Key, key)) return -1;

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

		void Remove(const char *key)
		{
			KeyMapItem *tmpitem = Root.FirstItem;

			while(true){
				if (!strcmp(tmpitem->Key, key)){
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

		ItemType Item(const char *key)
		{
			KeyMapItem *tmpitem = Root.FirstItem;

			//printk("Ricerco un elemento: %s\n", key);

			while(true){
				//printk("Elemento attuale: %s", tmpitem->Key);

				if (!strcmp(tmpitem->Key, key)){
					return tmpitem->ItemPtr;
				}

				if (tmpitem->NextItem != 0){
					tmpitem = tmpitem->NextItem;
				}else{
					printk("NON HO TROVATO NESSUN ELEMENTO (%s)!\n", key);

					return 0;
				}
			}
		}

		//HACK
		const char *Item(int key)
		{
			KeyMapItem *tmpitem = Root.FirstItem;

			int i = 0;
			while (i < key){
				if (tmpitem->NextItem != 0){
					tmpitem = tmpitem->NextItem;
				}else{
					printk("NON HO TROVATO NESSUN ELEMENTO (%i)!\n", key);

					return 0;
				}
				i++;
			}

			return tmpitem->Key;
		}

		ItemType operator[](const char *key)
		{
			return Item(key);
		}

		const char *operator[](int key)
		{
			return Item(key);
		}

	private:
		class KeyMapItem
		{
			public:
				KeyMapItem(const char *key, ItemType item)
				{
					PrevItem = 0;
					NextItem = 0;

					Key = strdup(key);
					ItemPtr = item;
				}

				KeyMapItem *PrevItem;
				KeyMapItem *NextItem;
				char *Key;
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
