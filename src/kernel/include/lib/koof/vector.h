/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: vector.h                                                        *
 *   Date: 05/12/2006                                                      *
 ***************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <core/printk.h>

#warning DEPRECATED DEPRECATED!!

template <class ItemType> class Vector
{
	public:
		Vector()
		{
			VectorSize = 0;
			NullItem = (ItemType) 0;

			Root.FirstItem = 0;
			Root.LastItem = 0;
		}

		Vector(ItemType nullItem)
		{
			VectorSize = 0;
			NullItem = nullItem;

			Root.FirstItem = 0;
			Root.LastItem = 0;
		}

		int Add(ItemType i)
		{
			VectorItem *item = new VectorItem(VectorSize, i);

			if (VectorSize == 0){
				Root.FirstItem = item;
				Root.LastItem = item;
			}else{
				VectorItem *tmpitem = Root.FirstItem;
		
				while(true){
					//printk("Item attuale: %s\n", tmpitem->Key);
					if (tmpitem->NextItem == 0){
						tmpitem->NextItem = item;
						item->PrevItem = tmpitem;

						break;
					}else{
						tmpitem = tmpitem->NextItem;
					}
				}
			}

			return VectorSize++;
		}

		//TODO: If the element doesn't exist it's better to return a good value
		void Remove(int index)
		{
			VectorItem *tmpitem = Root.FirstItem;

			if (tmpitem == 0){
				return; //TODO: Return a good value
			}

			while(true){
				if (tmpitem->Index == index){
					tmpitem->PrevItem->NextItem = tmpitem->NextItem;
					
					VectorSize--;

					free(tmpitem);

					return;

				}else if (tmpitem->NextItem != 0){
					tmpitem = tmpitem->NextItem;

				}else{
					//TODO: Uh?
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
			return VectorSize;
		}

		bool IsEmpty()
		{
			return (VectorSize == 0);
		}

		ItemType Item(int index)
		{
			VectorItem *tmpitem = Root.FirstItem;

			///printk("Ricerco un elemento: %i\n", index);
			if (tmpitem == 0){
				return NullItem;
			}

			while(true){
				///printk("Elemento attuale: %i", tmpitem->Index);

				if (tmpitem->Index == index){
					return tmpitem->ItemPtr;
				}

				if (tmpitem->NextItem != 0){
					tmpitem = tmpitem->NextItem;
				}else{
					///printk("NON HO TROVATO NESSUN ELEMENTO (%i)!\n", index);
					return NullItem;
				}
			}
		}

		ItemType operator[](int index)
		{
			return Item(index);
		}

		ItemType GetNullItem()
		{
			return NullItem;
		}

		void SetNullItem(ItemType nullItem)
		{
			NullItem = nullItem;
		}

	private:
		class VectorItem{
			public:
				VectorItem(int index, ItemType item)
				{
					PrevItem = 0;
					NextItem = 0;

					Index = index;
					ItemPtr = item;
				}

				VectorItem *PrevItem;
				VectorItem *NextItem;
				int Index;
				ItemType ItemPtr;
		};

		class VectorRootItem
		{
			public:
				VectorRootItem()
				{
					FirstItem = 0;
					LastItem = 0;
				}

				VectorItem *FirstItem;
				VectorItem *LastItem;
		};

		int VectorSize;
		VectorRootItem Root;
		ItemType NullItem;
};

#endif
