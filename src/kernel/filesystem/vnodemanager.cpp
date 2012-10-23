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
 *   Name: vnodemanager.cpp                                                *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#define ENABLE_DEBUG_MSG 0

#include <filesystem/vnodemanager.h>
#include <filesystem/vfs.h>

#include <core/printk.h>

#ifdef USE_GLIBC
#include <stdlib.h>
#endif
#include <string.h>

#include <debugmacros.h>

using namespace FileSystem;

QHash<VNodeId, VNode *>  VNodeManager::VNodes;

void VNodeManager::Init()
{
    VNodeManager::VNodes = QHash<VNodeId, VNode *>();
}

VNode *VNodeManager::LookupVnode(unsigned int mountId, unsigned long long id)
{
    DEBUG_MSG("VNodeManager::LookupVnode: (%i:%lli)\n", mountId, id);
  
    VNodeId vid(mountId, id);

    return VNodes.value(vid);
}

void VNodeManager::GetVnode(unsigned int mount_id, unsigned long long id, VNode **node)
{ 
	*node = LookupVnode(mount_id, id);

	if (*node == NULL){
		*node = NewVnode(mount_id, id);
	}

	if (*node != NULL) (*node)->refCount++;
	
	DEBUG_MSG("VNodeManager::GetVnode: (%i:%lli): new refcount: %i\n", mount_id, id, (*node)->refCount);
}

//FIXME: it might remove vnodes that are still in use
void VNodeManager::PutVnode(VNode *node)
{
#if 1
    DEBUG_MSG("VNodeManager::PutVnode: (%i:%lli): previous refCount: %i\n", node->vnid.mountId, node->vnid.id, node->refCount);
  
    node->refCount--;

    if (node->refCount == 0){
        DEBUG_MSG("VNodeManager::PutVnode: Deleting VNode: (%i:%lli)\n", node->vnid.mountId, node->vnid.id);
        //TODO: here we should lock VNodes and we should prevent vnodes creation
        VNodes.remove(node->vnid);
        ///TODO: here we can unlock VNodes
        if ((node->mount != NULL) && (node->mount->fs != NULL)){
            if (FS_CALL(node, closevnode)(node) < 0){
                //TODO: an error has occoured. we should do something here
            }
        }

        node->mount = (FSMount *) node->vnid.id; //TODO: I can't remember the meaning of this line
        delete node;
        //TODO: here we can allow vnodes creation again

        #if DEBUG == 1
            DEBUG_MSG("Active VNodes:\n");
            foreach (VNode *n, VNodes){
                DEBUG_MSG("VNode: (%i:%lli): refCount: %i\n", n->vnid.mountId, n->vnid.id, n->refCount);
            }
            DEBUG_MSG("---\n");
        #endif
    }
#endif

#if 0
    node->refCount--;

    if (node->refCount <= 0){
        printk("VNodeManager: %x should be removed\n", node);
    }
#endif
}

VNode *VNodeManager::ReferenceVnode(VNode *node)
{   
    node->refCount++;

    DEBUG_MSG("VNodeManager::ReferenceVnode: (%i:%lli): new refCount: %i\n", node->vnid.mountId, node->vnid.id, node->refCount);

    return node;
}

VNode *VNodeManager::NewVnode(unsigned int mountId, unsigned long long id)
{
    DEBUG_MSG("VNodeManager::NewVnode: (%i:%lli)\n", mountId, id);
  
    VNode *newnode = new VNode(mountId, id);
    if (newnode == NULL) return NULL;

    newnode->refCount = 0;
    newnode->mount = 0;
    newnode->coveredBy = 0;
    newnode->privdata = 0;

    VNodes.insert(newnode->vnid, newnode);
    
    return newnode;
}
