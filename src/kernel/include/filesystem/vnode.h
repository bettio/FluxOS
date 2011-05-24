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
 *   Name: vnode.h                                                         *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#ifndef _FILESYSTEM_VNODE_H_
#define _FILESYSTEM_VNODE_H_

#include <QHash>
#include <defs.h>

struct FSMount;

class VNodeId
{
    public:
        inline VNodeId()
        {
        }
      
        inline VNodeId(unsigned int _mountId, unsigned long long _id)
        {
            id = _id;
            mountId = _mountId;
        }

        unsigned long long id;
        unsigned int mountId;
};

inline uint qHash(VNodeId id)
{
    return id.id % (id.mountId + 1);
}

inline bool operator==(VNodeId id0, VNodeId id1)
{
    return (id0.id == id1.id) && (id0.mountId == id1.mountId);
}

class VNode
{
    public:
        inline VNode(unsigned int mountId, unsigned long long id)
        {
            CHECK_THIS_OBJECT()
            
            vnid = VNodeId(mountId, id);
            refCount = 0;
        }
      
        VNodeId vnid;
        unsigned int refCount;
        FSMount *mount;
        VNode *coveredBy;
        void *privdata;
};

#endif
