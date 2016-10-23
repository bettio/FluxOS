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
 *   Name: vfs.cpp                                                         *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#define ENABLE_DEBUG_MSG 0

#include <filesystem/pipe.h>
#include <filesystem/vnodemanager.h>
#include <drivers/blockdevicemanager.h>
#include <core/printk.h>
#include <debugmacros.h>

#include <cstring.h>
#include <cstdlib.h>

#include <QHash>
#include <QLinkedList>
#include <QString>

#include <filesystem/vfs.h>

#define MAX_COUNT 256

///#define VFS_DEBUG 1

using namespace FileSystem;

VNode *VFS::RootNode = 0;
unsigned int VFS::FirstFreeMntID;
QHash<QString, FileSystemInfo *> VFS::FileSystems;

void VFS::Init()
{
    FileSystems = QHash<QString, FileSystemInfo *>();
    VNodeManager::Init();
    Pipe::init();
}

int VFS::RegisterFileSystem(FileSystemInfo *fsinfo)
{
    if (fsinfo == NULL) {
	printk("FileSystemInfo parameter is NULL. FS registration failed.\n");
        return -1;
    }

    FileSystems.insert(fsinfo->name, fsinfo);
    printk("Registered %s filesystem\n", fsinfo->name);

    return 0;
}

int VFS::UnregisterFileSystem(const char *name)
{
	if (FileSystemInfo *fsinfo = FileSystems.take(name)){
        delete fsinfo;
        return 0;

    }else{
        return -EINVAL;
    }
}

int VFS::Mount(const char *source, const char *mountpoint, const char *fstype, unsigned int flags, const void *data)
{
	const FileSystemInfo *fsinfo = FileSystems[fstype];
	if (fsinfo == NULL){
        printk("Unknown filesystem type\n");
		return -ENODEV;
	}

	if (RootNode == 0){
		if (!strcmp(mountpoint, "/")){ //strncmp
			printk("Mounting root filesystem.\n");

			FSMount *fsmount = new FSMount;
			if (fsmount == NULL) return -ENOMEM;
			fsmount->mountId = FirstFreeMntID;

			if (!strncmp(source, "/dev/", 5)){
				BlockDevice *blkDev = BlockDeviceManager::Device((char *) source + 5);
				if (blkDev == NULL) return -ENOENT;
				fsinfo->mount(fsmount, blkDev);
			}else{
				fsinfo->mount(fsmount, 0);
			}

			RootNode = fsmount->fsRootVNode;
			fsmount->coversVNode = 0; //ROOT doesn't conver any node
		}else{
			printk("Mount / before mounting any other filesystem!\n");
			//TODO: return a better value
			return -1;
		}

	}else{
		VNode *mntnode;

        //TODO: warning: here a vnode is referenced
        //TODO: missing support to relative paths
		int ret = RelativePathToVnode(0, mountpoint, &mntnode);
		if (ret < 0) return ret;

		FSMount *fsmount = new FSMount;
		if (fsmount == NULL) return -ENOMEM;
		fsmount->mountId = FirstFreeMntID;

		if (!strncmp(source, "/dev/", 5)){
			BlockDevice *blkDev = BlockDeviceManager::Device((char *) source + 5);
			if (blkDev == NULL) return -ENOENT;
			fsinfo->mount(fsmount, blkDev);
		}else{
			fsinfo->mount(fsmount, 0);
		}

		mntnode->coveredBy = fsmount->fsRootVNode;
		fsmount->coversVNode = mntnode;
	}

	printk("Mounted %s on %s\n", source, mountpoint);

	#ifdef VFS_DEBUG
		printk("Mount id: %i.\n", FirstFreeMntID);
	#endif

	FirstFreeMntID++;

	return 0;
}

int VFS::Umount(const char *mountpoint)
{
    VNode *node;
    //TODO: missing support to relative paths
    int retVal = RelativePathToVnode(0, mountpoint, &node);
    if (retVal < 0){
        return retVal; 
    }
    
    FSMount *mount = node->mount;
    if (mount->coversVNode){
        VNodeManager::PutVnode(mount->coversVNode);
    }
    
    retVal = FS_CALL(node, umount)(node);
    if (retVal < 0){
        VNodeManager::PutVnode(node);
        return retVal; 
    }
    
    VNodeManager::PutVnode(node);
    VNodeManager::PutVnode(node);
    
    delete mount;

    if (node == RootNode){
        RootNode = 0;
    }

    return 0;
}

//TODO: stringhe vuote
int VFS::RelativePathToVnode(VNode *start, const char *_path, VNode **node, bool traverse_leaf_link, int count)
{
    char *pathClone = strdup(_path);
    char *path = pathClone;
    if (path == NULL){
        return -ENOMEM;    
    }
    
    DEBUG_MSG("VFS::RelativePathToVnode(%p, %s, %p, %i, %i)\n", start, _path, *node, traverse_leaf_link, count);

    //In case of absolute (which begin with /) path use RootNode as start
    if (path[0] == '/'){
        start = RootNode;
        while (*++path == '/'); //TODO: it might be removed
    } else if (IS_NULL_PTR(start)) {
        printk("Error: VFS::RelativePathToVnode: null pointer detected\n");
        return -EINVAL;
    }

    char *nextPath;
    VNode *nextVnode;
    VNode *tmpnode;
    int result;

    if (count == MAX_COUNT){
        free(pathClone);
        return -ELOOP;
    }

    tmpnode = start;
    VNodeManager::ReferenceVnode(start);

    while(1){
        if (path[0] == '\0'){
            break;
        }

        for (nextPath = path + 1; *nextPath != '\0' && *nextPath != '/'; nextPath++);

        if (*nextPath == '/') {
            *nextPath = '\0';
            do{
                nextPath++;
            }while (*nextPath == '/');
        }

        unsigned int NodeType;

        DEBUG_MSG("Resolving path component: %s\n", path);

        if (!strcmp(path, "..") && tmpnode->mount->coversVNode){
            DEBUG_MSG("Going back to the point where the filesystem has been mounted\n");
            VNodeManager::PutVnode(tmpnode);
            tmpnode = VNodeManager::ReferenceVnode(tmpnode->mount->coversVNode);
        }

        #ifdef USE_TASKS
            result = FS_CALL(tmpnode, access)(tmpnode, X_OK, Task::CurrentTask()->Uid, Task::CurrentTask()->Gid);
            if (result < 0){
                DEBUG_MSG("Path component access error\n");
                *node = 0;
                VNodeManager::PutVnode(tmpnode);
                free(pathClone);
                return result;
            }
        #else
            result = 0;
        #endif
        
        result = FS_CALL(tmpnode, lookup)(tmpnode, path, &nextVnode, &NodeType);
        if (result < 0){
            DEBUG_MSG("Error: failed file lookup\n");
            *node = 0;
            VNodeManager::PutVnode(tmpnode);
            free(pathClone);
            return result;
        }

        //A regular file is used as a path component instead of a directory
        if (S_ISREG(NodeType) && (nextPath[0] != 0)){
            *node = 0;
            VNodeManager::PutVnode(tmpnode);
            free(pathClone);
            return -ENOTDIR;

        }else if (S_ISLNK(NodeType) && !(!traverse_leaf_link && nextPath[0] == '\0')){
            //TODO: Change malloc size
            char *tmplnkbuf = (char *) malloc(20480);
            if (tmplnkbuf == NULL){
                node = 0;
                VNodeManager::PutVnode(tmpnode);
                free(pathClone);
                return -ENOMEM;
            }
            result = FS_CALL(nextVnode, readlink)(nextVnode, tmplnkbuf, 20480);
            if (result < 0){
                node = 0;
                VNodeManager::PutVnode(tmpnode);
                free(pathClone);
                return result;                
            }
            
            result = RelativePathToVnode(tmpnode, tmplnkbuf, &nextVnode, traverse_leaf_link, count + 1);

            free(tmplnkbuf);

            if (result < 0){
                *node = 0;
                VNodeManager::PutVnode(tmpnode);
                free(pathClone);
                return result;
            }
        }

        if (nextVnode->coveredBy){
            VNodeManager::ReferenceVnode(nextVnode->coveredBy);
            VNode *prevNode = nextVnode;
            nextVnode = nextVnode->coveredBy;
            VNodeManager::PutVnode(prevNode); //TODO: check here better
        }

        VNodeManager::PutVnode(tmpnode);
        tmpnode = nextVnode;
        path = nextPath;
    }

    *node = tmpnode;
    free(pathClone);

    return 0;
}

int VFS::GetDirPathFromVnode(VNode *node, char **pathFromVnode)
{
    VNode *tmpnode;
    QLinkedList<char *> pathStack;
    char *dirName;
    int pathLen = 2;
    
    VNodeManager::ReferenceVnode(node);
    do{
        if (node->mount->coversVNode){
            VNode *coveredVNode = node->mount->coversVNode;
            VNodeManager::ReferenceVnode(coveredVNode);
            VNodeManager::PutVnode(node);
            node = coveredVNode;
        }

        unsigned int NodeType;
        int retVal = FS_CALL(node, lookup)(node, "..", &tmpnode, &NodeType);
        if (retVal < 0){
            VNodeManager::PutVnode(node);
            return retVal;
        }

        int nameLen;
        retVal = FS_CALL(tmpnode, name)(tmpnode, node, &dirName, &nameLen);
        if (retVal < 0){
            VNodeManager::PutVnode(node);
            VNodeManager::PutVnode(tmpnode);
            return retVal;
        }
        pathStack.append(dirName);
        pathLen += nameLen + 1;

        VNodeManager::PutVnode(node);
        node = tmpnode;
    }while (strcmp(".", dirName));
    VNodeManager::PutVnode(tmpnode);
    pathStack.takeLast();
    
    char *path = (char *) malloc(pathLen);
    path[0] = '/';
    path[1] = '\0';

    int i = 1;
    while (pathStack.size()){
        char *name = pathStack.takeLast();
        strcpy(path + i, name);
        if (pathStack.size()){
            i += strlen(name) + 1;
            path[i - 1] = '/';
        }
        free(name);
    }

    *pathFromVnode = path;
    return 0;
}
