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
 *   Name: fscalls.cpp                                                     *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#define ENABLE_DEBUG_MSG 0

#include <filesystem/fscalls.h>

#include <debugmacros.h>
#include <cstdlib.h>
#include <core/printk.h>
#include <filesystem/filedescriptor.h>
#include <task/scheduler.h>
#include <filesystem/pipe.h>
#include <filesystem/vnodemanager.h>

using namespace FileSystem;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define MAX_FILENAME_LEN 8192

#define ENAMETOOLONG 99; //HACK FIXME
#define ERANGE 100;

#define CHECK_FOR_EBADF(fd) \
if (!((fd >= 0) && (fd < Scheduler::currentThread()->parentProcess->openFiles->size()))) return -EBADF;

int isValidFileName(const char *name)
{
    int len = strlen(name);

    if (name[0] == '\0'){
        return -ENOENT;
    }else if (len >=8192){
        return -ENAMETOOLONG;
    }else{
        return 0;
    }
}

int isValidUserFileName(const char *name)
{
    int retVal = isValidFileName(name);
    
    if (retVal < 0){
        return retVal;
    }

    //TODO: we should check . and ..

    return 0;
}

//TODO: testare se un file descriptor e` valido

int getcwd(char *buf, size_t size)
{
    char *path;
    int retVal = VFS::GetDirPathFromVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, &path);
    if (retVal < 0){
        return retVal;
    }
    if (size < strlen(path)){
        if (size >= 1){
            buf[0] = '\0';
        }
        return -ERANGE;
    }else{
        strncpy(buf, path, size);
        free(path);
        return 0;
    }
}


//TODO: We must check if the path is a file path
int chdir(const char *path)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node);

	if (result < 0) return result;

	Scheduler::currentThread()->parentProcess->currentWorkingDirNode = node;

	return 0;
}

int stat(const char *path, struct stat *buf)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node);

	if (result >= 0) result = FS_CALL(node, stat)(node, buf);

	//TODO: Alla fine la memoria va` liberata?

	return result;
}

int lstat(const char *path, struct stat *buf)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node, false);	

	if (result >= 0) result = FS_CALL(node, stat)(node, buf);

	return result;
}

int fstat(int filedes, struct stat *buf)
{
        CHECK_FOR_EBADF(filedes);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(filedes);
	if (fdesc == NULL) return -EBADF;

	VNode *tmpnode = fdesc->node;

	return FS_CALL(tmpnode, stat)(tmpnode, buf);
}

int stat64(const char *path, struct stat64 *buf64)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node);

	struct stat buf;

	if (result >= 0) result = FS_CALL(node, stat)(node, &buf);

	buf64->st_dev = buf.st_dev;
	buf64->st_ino = buf.st_ino;
	buf64->st_mode = buf.st_mode;
	buf64->st_nlink = buf.st_nlink;
	buf64->st_uid = buf.st_uid;
	buf64->st_gid = buf.st_gid;
	buf64->st_rdev = buf.st_rdev;
	buf64->st_size = buf.st_size;
	buf64->st_blksize = buf.st_blksize;
	buf64->st_blocks = buf.st_blocks;
	buf64->st_atime = buf.st_atime;
	buf64->st_mtime = buf.st_mtime;
	buf64->st_ctime = buf.st_ctime;

	//TODO: Alla fine la memoria va` liberata?

	return result;
}

int lstat64(const char *path, struct stat64 *buf64)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node, false);

	struct stat buf;

	if (result >= 0) result = FS_CALL(node, stat)(node, &buf);

	buf64->st_dev = buf.st_dev;
	buf64->st_ino = buf.st_ino;
	buf64->st_mode = buf.st_mode;
	buf64->st_nlink = buf.st_nlink;
	buf64->st_uid = buf.st_uid;
	buf64->st_gid = buf.st_gid;
	buf64->st_rdev = buf.st_rdev;
	buf64->st_size = buf.st_size;
	buf64->st_blksize = buf.st_blksize;
	buf64->st_blocks = buf.st_blocks;
	buf64->st_atime = buf.st_atime;
	buf64->st_mtime = buf.st_mtime;
	buf64->st_ctime = buf.st_ctime;

	//TODO: Alla fine la memoria va` liberata?

	return result;
}

int fstat64(int filedes, struct stat64 *buf64)
{
	struct stat buf;

        CHECK_FOR_EBADF(filedes);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(filedes);
	if (fdesc == NULL) return -EBADF;

	VNode *tmpnode = fdesc->node;

	int result =  FS_CALL(tmpnode, stat)(tmpnode, &buf);

	buf64->st_dev = buf.st_dev;
	buf64->st_ino = buf.st_ino;
	buf64->st_mode = buf.st_mode;
	buf64->st_nlink = buf.st_nlink;
	buf64->st_uid = buf.st_uid;
	buf64->st_gid = buf.st_gid;
	buf64->st_rdev = buf.st_rdev;
	buf64->st_size = buf.st_size;
	buf64->st_blksize = buf.st_blksize;
	buf64->st_blocks = buf.st_blocks;
	buf64->st_atime = buf.st_atime;
	buf64->st_mtime = buf.st_mtime;
	buf64->st_ctime = buf.st_ctime;

	//TODO: Alla fine la memoria va` liberata?

	return result;
}

int readlink(const char *path, char *buf, size_t bufsiz)
{
	VNode *tmpnode;

	int result = VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, path, &tmpnode, false);

	if (result < 0) return result;

	return FS_CALL(tmpnode, readlink)(tmpnode, buf, bufsiz);
}

int getdents(int fd, dirent *dirp, unsigned int count)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, getdents)(fdesc->node, dirp, count);
}

int access(const char *pathname, int mode)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) pathname, &node, false);

	if (result >= 0) result = FS_CALL(node, access)(node, mode, Scheduler::currentThread()->parentProcess->uid, Scheduler::currentThread()->parentProcess->gid);

	return result;
}

int createNewFile(const char *pathname, mode_t mode, VNode **node)
{
    char *name;
    
    int result = pathToParentAndName(pathname, node, &name);
    if (result < 0){
        return result;
    }
    result = isValidUserFileName(name);
    if (result < 0){
        return result;
    }
    
    result = FS_CALL((*node), creat)((*node), name, 0);
    
    free(name);
    
    return result;
}

#define O_CREAT		   0100	/* not fcntl */

//TODO: implementare la modalita` append
int open(const char *pathname, int flags)
{
	int result;

	VNode *node;

	result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, pathname, &node, true);

	if (result < 0){
        if (flags & O_CREAT){
			createNewFile(pathname, flags, &node); //le stesse flag?
        }else{
		    DEBUG_MSG("Failed to open %s\n", pathname);

		    return result;
        }
	}

	FileDescriptor *fdesc = new FileDescriptor(node);

	return Scheduler::currentThread()->parentProcess->openFiles->add(fdesc);
}

int close(ProcessControlBlock *process, int fd)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;
 
    Scheduler::currentThread()->parentProcess->openFiles->remove(fd);

    VNodeManager::PutVnode(fdesc->node);
    delete fdesc;

    return 0;
}

int close(int fd)
{
    return close(Scheduler::currentThread()->parentProcess, fd);
}

int write(int fd, const void *buf, size_t count)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	int ret = FS_CALL(fdesc->node, write)(fdesc->node, fdesc->fpos, (char *) buf, count);

	if (ret > 0) fdesc->fpos += ret;

	return ret;
}

int read(int fd, void *buf, size_t count)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	int ret = FS_CALL(fdesc->node, read)(fdesc->node, fdesc->fpos, (char *) buf, count);

	if (ret > 0) fdesc->fpos += ret;

	return ret;
}

int lseek(int fd, off_t offset, int whence)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;

    //TODO: we should check here if it is possible to lseek (seek on pipes is not possible)

	if (whence == SEEK_SET){ 
		fdesc->fpos = offset;
	}else if (whence == SEEK_CUR){
		fdesc->fpos += offset;
	}else if (whence == SEEK_END){
		//TODO: Size - offset
	}

	return fdesc->fpos;
}

int fsync(int fd)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, fsync)(fdesc->node);
}

int fdatasync(int fd)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, fdatasync)(fdesc->node);
}

//NOTE: 64 bit implementation
int truncate(const char *path, uint64_t length)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node);

	if (result >= 0) result = FS_CALL(node, truncate)(node, length);

	return result;
}

//NOTE: 64 bit implementation
int ftruncate(int fd, uint64_t length)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, truncate)(fdesc->node, length);
}

int chmod(const char *path, mode_t mode)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node);

	if (result >= 0) result = FS_CALL(node, chmod)(node, mode);

	return result;
}

int fchmod(int fildes, mode_t mode)
{
        CHECK_FOR_EBADF(fildes);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fildes);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, chmod)(fdesc->node, mode);
}

int chown(const char *path, uid_t owner, gid_t group)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node);

	if (result >= 0) result = FS_CALL(node, chown)(node, owner, group);

	return result;
}

int fchown(int fd, uid_t owner, gid_t group)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, chown)(fdesc->node, owner, group);
}

int lchown(const char *path, uid_t owner, gid_t group)
{
	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, (char *) path, &node, false);

	if (result >= 0) result = FS_CALL(node, chown)(node, owner, group);

	return result;
}

int pread(int fd, void *buf, size_t count, uint64_t offset)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, read)(fdesc->node, offset, (char *) buf, count);
}

int pwrite(int fd, const void *buf, size_t count, uint64_t offset)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, write)(fdesc->node, offset, (char *) buf, count);
}

//TODO
int fcntl(int fd, int cmd, long arg)
{
	return -EINVAL;
}

int ioctl(int d, int request, long arg)
{
        CHECK_FOR_EBADF(d);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(d);
	if (fdesc == NULL) return -EBADF;

	printk("---ioctl\n");

	if (FS_CALL(fdesc->node, ioctl) != 0){
		return FS_CALL(fdesc->node, ioctl)(fdesc->node, request, arg);
	}else{
		printk("ioctl non implementata\n");
		return 0;
	}
}

int pathToParentAndName(const char *pathname, VNode **parentDirectory, char **name)
{
    int currentPathToken = 0;
    int i = 0;
    while(pathname[i]){
        if ((pathname[i]  == '/') && (pathname[i + 1] != '\0')){
             currentPathToken = i + 1;
        }
        i++;
    }
    *name = strndup(&pathname[currentPathToken], i - currentPathToken);
    if ((*name)[0] == '\0') return -ENOENT;
    if ((*name)[i - currentPathToken - 1] == '/'){
        (*name)[i - currentPathToken - 1] = '\0';
    }

    if (currentPathToken - 1 > 0){
        char *tmpPath = strndup(pathname, currentPathToken - 1);
        int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, tmpPath, &(*parentDirectory), true);
        free(tmpPath);
        if (result < 0){
            free(name);
            *name = 0;
            return result;
        }
        
    }else{
        *parentDirectory = VNodeManager::ReferenceVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode);
    }
    
    return 0;
}

int creat(const char *pathname, mode_t mode)
{
    VNode *node;
    
    createNewFile(pathname, mode, &node);
    return open(pathname, 0);
}


int utime(const char *filename, const struct utimbuf *buf)
{
    VNode *tmpnode;

    int result = VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, filename, &tmpnode, false);

    if (result < 0) return result;

    return FS_CALL(tmpnode, utime)(tmpnode, buf);
}

int statfs(const char *path, struct statfs *buf)
{
    VNode *tmpnode;

    int result = VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, path, &tmpnode, false);

    if (result < 0) return result;

    return FS_CALL(tmpnode, statfs)(tmpnode, buf);
}

int fstatfs(int fd, struct statfs *buf)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;

    return FS_CALL(fdesc->node, statfs)(fdesc->node, buf);
}

int unlink(const char *pathname)
{
    VNode *node;
    char *name;
    
    int result = pathToParentAndName(pathname, &node, &name);
    if (result < 0){
      return result;
    }
    result = isValidUserFileName(name);
    if (result < 0){
        return result;
    }

    result = FS_CALL(node, unlink)(node, name);
    
    VNodeManager::PutVnode(node);
    free(name);
    
    return result;
}

int link(const char *oldpath, const char *newpath)
{
    VNode *oldNode;
    int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, oldpath, &oldNode, false);
    if (result < 0){
        return result;
    }
    
    VNode *node;
    char *name;
    result = pathToParentAndName(newpath, &node, &name);
    if (result < 0){
      VNodeManager::PutVnode(oldNode);
      return result;
    }
    
    result = FS_CALL(node, link)(node, oldNode, name);
    
    VNodeManager::PutVnode(oldNode);
    VNodeManager::PutVnode(node);
    free(name);
    
    return result;
}

int symlink(const char *oldpath, const char *newpath)
{
    VNode *node;
    char *name;
    
    int result = pathToParentAndName(newpath, &node, &name);
    if (result < 0){
      return result;
    }
    
    result = FS_CALL(node, symlink)(node, oldpath, name);
    
    VNodeManager::PutVnode(node);
    free(name);
    
    return result;  
}

int mknod(const char *pathname, mode_t mode, dev_t dev)
{
    VNode *node;
    char *name;
    
    int result = pathToParentAndName(pathname, &node, &name);
    if (result < 0){
      return result;
    }
    result = isValidUserFileName(name);
    if (result < 0){
        return result;
    }

    result = FS_CALL(node, mknod)(node, name, mode, dev);
    
    VNodeManager::PutVnode(node);
    free(name);
    
    return result;
}

int mkdir(const char *pathname, mode_t mode)
{
    VNode *node;
    char *name;

    int result = pathToParentAndName(pathname, &node, &name);
    if (result < 0){
      return result;
    }
    result = isValidUserFileName(name);
    if (result < 0){
        return result;
    }

    result = FS_CALL(node, mkdir)(node, name, mode);
    
    VNodeManager::PutVnode(node);
    free(name);
    
    return result;
}

int rmdir(const char *pathname)
{
    VNode *node;
    char *name;
    
    int result = pathToParentAndName(pathname, &node, &name);
    if (result < 0){
      return result;
    }
    result = isValidUserFileName(name);
    if (result < 0){
        return result;
    }

    result = FS_CALL(node, rmdir)(node, name);
    
    VNodeManager::PutVnode(node);
    free(name);
    
    return result;
}

int rename(const char *oldpath, const char *newpath)
{
    VNode *oldNode;
    char *oldName;
    int result = pathToParentAndName(oldpath, &oldNode, &oldName);
    if (result < 0){
      return result;
    }
    result = isValidUserFileName(oldName);
    if (result < 0){
        return result;
    }

    VNode *newNode;
    char *newName;
    result = pathToParentAndName(newpath, &newNode, &newName);
    if (result < 0){
      VNodeManager::PutVnode(oldNode);
      free(oldName);
      return result;
    }
    result = isValidUserFileName(newName);
    if (result < 0){
        return result;
    }

    //oldpath and newpath should belong to the same mountpoint
    if ((oldNode->vnid.mountId != newNode->vnid.mountId) ||
        (oldNode->mount->fs->rename != newNode->mount->fs->rename)){
        result = -EXDEV;
    }else{
        result = FS_CALL(newNode, rename)(oldNode, oldName, newNode, newName);
    }
   
    VNodeManager::PutVnode(oldNode);
    VNodeManager::PutVnode(newNode);
    free(oldName);
    free(newName);
    
    return result;
}

int dup(int oldfd)
{
    CHECK_FOR_EBADF(oldfd);
    FileDescriptor *oldFdesc = Scheduler::currentThread()->parentProcess->openFiles->at(oldfd);
    if (oldFdesc == NULL) return -EBADF;
    
    FileDescriptor *fdesc = new FileDescriptor(VNodeManager::ReferenceVnode(oldFdesc->node));
    fdesc->fpos = oldFdesc->fpos;
    
    return Scheduler::currentThread()->parentProcess->openFiles->add(fdesc);
}

//TODO: Implement dup2
int dup2(int oldfd, int newfd)
{
    return -EINVAL;
}

//TODO: check pipefd address
int pipe(int pipefd[2])
{
    VNode *node = Pipe::newPipe();
    if (node == NULL) return -ENOMEM;

    //read end
    FileDescriptor *fdesc0 = new FileDescriptor(node);
    pipefd[0] = Scheduler::currentThread()->parentProcess->openFiles->add(fdesc0);

    //write end
    FileDescriptor *fdesc1 = new FileDescriptor(VNodeManager::ReferenceVnode(node));
    pipefd[1] = Scheduler::currentThread()->parentProcess->openFiles->add(fdesc1);

    return 0;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    if (!((fd >= 0) && (fd < Scheduler::currentThread()->parentProcess->openFiles->size()))) return (void *) -EBADF;
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return (void *) -EBADF;

    void *ret = FS_CALL(fdesc->node, mmap)(fdesc->node, addr, length, prot, flags, fd, offset);

    return ret;
}

