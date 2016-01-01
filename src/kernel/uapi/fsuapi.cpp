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

#include <uapi/fsuapi.h>

#include <debugmacros.h>
#include <cstdlib.h>
#include <core/printk.h>
#include <filesystem/filedescriptor.h>
#include <filesystem/pollfd.h>
#include <task/eventsmanager.h>
#include <task/scheduler.h>
#include <filesystem/ioctl.h>
#include <filesystem/pipe.h>
#include <filesystem/socket.h>
#include <filesystem/statfs.h>
#include <filesystem/utimbuf.h>
#include <filesystem/vnodemanager.h>
#include <filesystem/pollfd.h>
#include <mm/usermemoryops.h>

using namespace FileSystem;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define MAX_FILENAME_LEN 8192

#define ENAMETOOLONG 99; //HACK FIXME
#define ERANGE 100;

#define CHECK_FOR_EBADF(fd) \
if (!((fd >= 0) && (fd < Scheduler::currentThread()->parentProcess->openFiles->size()))) return -EBADF;

int FSUAPI::isValidFileName(const char *name)
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

int FSUAPI::isValidUserFileName(const char *name)
{
    int retVal = isValidFileName(name);
    
    if (retVal < 0){
        return retVal;
    }

    //TODO: we should check . and ..

    return 0;
}

//TODO: testare se un file descriptor e` valido

int FSUAPI::getcwd(char *buf, size_t size)
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

int FSUAPI::chdir(userptr const char *path)
{
    UserString dirPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!dirPath.isValid())) {
        return dirPath.errorCode();
    }

    VNode *node;
    int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, dirPath.data(), &node);
    if (result < 0) return result;
    int type = 0;
    int retVal = FS_CALL(node, type)(node, &type);
    if (retVal < 0){
        VNodeManager::PutVnode(node);
        return retVal;
    }
    if ((type & S_IFDIR) == 0){
        VNodeManager::PutVnode(node);
        return -ENOTDIR;
    }

    Scheduler::currentThread()->parentProcess->currentWorkingDirNode = node;

    return 0;
}


int FSUAPI::fchdir(int fd)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;
    int type = 0;
    int retVal = FS_CALL(fdesc->node, type)(fdesc->node, &type);
    if (retVal < 0) return retVal;
    if ((type & S_IFDIR) == 0) return -ENOTDIR;

    Scheduler::currentThread()->parentProcess->currentWorkingDirNode = VNodeManager::ReferenceVnode(fdesc->node);

    return 0;
}

int FSUAPI::stat(userptr const char *path, struct stat *buf)
{
    UserString fPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *node;
    int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node);
    if (UNLIKELY(result < 0)) {
        return result;
    }

    struct stat s;
    result = FS_CALL(node, stat)(node, &s);

    VNodeManager::PutVnode(node);

    if (UNLIKELY(result < 0)) {
        return result;
    }

    int ret = memcpyToUser(buf, &s, sizeof(struct stat));
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    return result;
}

int FSUAPI::lstat(userptr const char *path, struct stat *buf)
{
    UserString fPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *node;
    int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node, false);	
    if (UNLIKELY(result < 0)) {
        return result;
    }

    struct stat s;
    result = FS_CALL(node, stat)(node, &s);

    VNodeManager::PutVnode(node);

    if (UNLIKELY(result < 0)) {
        return result;
    }

    int ret = memcpyToUser(buf, &s, sizeof(struct stat));
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    return result;
}

int FSUAPI::fstat(int filedes, struct stat *buf)
{
    CHECK_FOR_EBADF(filedes);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(filedes);
    if (fdesc == NULL) {
        return -EBADF;
    }
    VNode *tmpnode = VNodeManager::ReferenceVnode(fdesc->node);

    struct stat s;
    int result = FS_CALL(tmpnode, stat)(tmpnode, &s);

    VNodeManager::PutVnode(tmpnode);

    if (UNLIKELY(result < 0)) {
        return result;
    }

    int ret = memcpyToUser(buf, &s, sizeof(struct stat));
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    return result;
}

int FSUAPI::stat64(userptr const char *path, struct stat64 *buf64)
{
    UserString fPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *node;
    int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node);
    if (UNLIKELY(result < 0)) {
        return result;
    }

    struct stat buf;
    result = FS_CALL(node, stat)(node, &buf);

    VNodeManager::PutVnode(node);

    if (UNLIKELY(result < 0)) {
        return result;
    }

    struct stat64 s64;
    s64.st_dev = buf.st_dev;
    s64.st_ino = buf.st_ino;
    s64.st_mode = buf.st_mode;
    s64.st_nlink = buf.st_nlink;
    s64.st_uid = buf.st_uid;
    s64.st_gid = buf.st_gid;
    s64.st_rdev = buf.st_rdev;
    s64.st_size = buf.st_size;
    s64.st_blksize = buf.st_blksize;
    s64.st_blocks = buf.st_blocks;
    s64.st_atime = buf.st_atime;
    s64.st_mtime = buf.st_mtime;
    s64.st_ctime = buf.st_ctime;

    int ret = memcpyToUser(buf64, &s64, sizeof(struct stat64));
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    return result;
}

int FSUAPI::lstat64(userptr const char *path, struct stat64 *buf64)
{
    UserString fPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *node;
    int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node, false);
    if (UNLIKELY(result < 0)) {
        return result;
    }

    struct stat buf;
    result = FS_CALL(node, stat)(node, &buf);

    VNodeManager::PutVnode(node);

    if (UNLIKELY(result < 0)) {
        return result;
    }

    struct stat64 s64;
    s64.st_dev = buf.st_dev;
    s64.st_ino = buf.st_ino;
    s64.st_mode = buf.st_mode;
    s64.st_nlink = buf.st_nlink;
    s64.st_uid = buf.st_uid;
    s64.st_gid = buf.st_gid;
    s64.st_rdev = buf.st_rdev;
    s64.st_size = buf.st_size;
    s64.st_blksize = buf.st_blksize;
    s64.st_blocks = buf.st_blocks;
    s64.st_atime = buf.st_atime;
    s64.st_mtime = buf.st_mtime;
    s64.st_ctime = buf.st_ctime;

    int ret = memcpyToUser(buf64, &s64, sizeof(struct stat64));
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    return result;
}

int FSUAPI::fstat64(int filedes, struct stat64 *buf64)
{
    CHECK_FOR_EBADF(filedes);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(filedes);
    if (fdesc == NULL) {
        return -EBADF;
    }

    VNode *tmpnode = VNodeManager::ReferenceVnode(fdesc->node);

    struct stat buf;
    int result = FS_CALL(tmpnode, stat)(tmpnode, &buf);

    VNodeManager::PutVnode(tmpnode);

    if (UNLIKELY(result < 0)) {
        return result;
    }

    struct stat64 s64;
    s64.st_dev = buf.st_dev;
    s64.st_ino = buf.st_ino;
    s64.st_mode = buf.st_mode;
    s64.st_nlink = buf.st_nlink;
    s64.st_uid = buf.st_uid;
    s64.st_gid = buf.st_gid;
    s64.st_rdev = buf.st_rdev;
    s64.st_size = buf.st_size;
    s64.st_blksize = buf.st_blksize;
    s64.st_blocks = buf.st_blocks;
    s64.st_atime = buf.st_atime;
    s64.st_mtime = buf.st_mtime;
    s64.st_ctime = buf.st_ctime;

    int ret = memcpyToUser(buf64, &s64, sizeof(struct stat64));
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    return result;

}

int FSUAPI::readlink(userptr const char *path, char *buf, size_t bufsiz)
{
    UserString linkPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!linkPath.isValid())) {
        return linkPath.errorCode();
    }

	VNode *tmpnode;

	int result = VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, linkPath.data(), &tmpnode, false);

	if (result < 0) return result;

	return FS_CALL(tmpnode, readlink)(tmpnode, buf, bufsiz);
}

int FSUAPI::getdents(int fd, dirent *dirp, unsigned int count)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, getdents)(fdesc->node, dirp, count);
}

int FSUAPI::access(userptr const char *pathname, int mode)
{
    UserString fPath(pathname, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node, false);

	if (result >= 0) result = FS_CALL(node, access)(node, mode, Scheduler::currentThread()->parentProcess->uid, Scheduler::currentThread()->parentProcess->gid);

	return result;
}

int FSUAPI::createNewFile(const char *pathname, mode_t mode, VNode **node)
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

mode_t FSUAPI::umask(mode_t mode)
{
    ProcessControlBlock *process = Scheduler::currentThread()->parentProcess;
    mode_t oldMask = process->umask;
    process->umask = mode & 0777;
    return oldMask;
}

int FSUAPI::openAtVNode(VNode *dirNode, userptr const char *pathname, int flags)
{
    UserString path(pathname, MAX_FILENAME_LEN);
    if (UNLIKELY(!path.isValid())) {
        return path.errorCode();
    }

    int result;
    VNode *node;
    result = FileSystem::VFS::RelativePathToVnode(dirNode, path.constData(), &node, true);

    if (result < 0){
        if (flags & O_CREAT){
            createNewFile(path.constData(), flags, &node);

        }else{
            DEBUG_MSG("Failed to open %s\n", path.constData());
            return result;
        }
    }
    
    FileDescriptor *fdesc = new FileDescriptor(node);
    fdesc->flags = flags;

    return Scheduler::currentThread()->parentProcess->openFiles->add(fdesc);
}

int FSUAPI::open(const char *pathname, int flags)
{
    return openAtVNode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, pathname, flags);
}

int FSUAPI::openat(int dirfd, const char *pathname, int flags)
{
    CHECK_FOR_EBADF(dirfd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(dirfd);
    if (fdesc == NULL) return -EBADF;
    int type = 0;
    int retVal = FS_CALL(fdesc->node, type)(fdesc->node, &type);
    if (retVal < 0) return retVal;
    if ((type & S_IFDIR) == 0) return -ENOTDIR;

    return openAtVNode(fdesc->node, pathname, flags);
}

int FSUAPI::closePB(ProcessControlBlock *process, int fd)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;
 
    Scheduler::currentThread()->parentProcess->openFiles->remove(fd);

    VNodeManager::PutVnode(fdesc->node);
    delete fdesc;

    return 0;
}

int FSUAPI::close(int fd)
{
    return closePB(Scheduler::currentThread()->parentProcess, fd);
}

int FSUAPI::write(int fd, const void *buf, size_t count)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;
    if ((fdesc->flags & O_ACCMODE) == O_RDONLY) return -EBADF;

    int ret = FS_CALL(fdesc->node, write)(fdesc->node, fdesc->fpos, (char *) buf, count);

    if (ret > 0) fdesc->fpos += ret;

    return ret;
}

int FSUAPI::read(int fd, void *buf, size_t count)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;
    if ((fdesc->flags & O_ACCMODE) == O_WRONLY) return -EBADF;

    int ret = FS_CALL(fdesc->node, read)(fdesc->node, fdesc->fpos, (char *) buf, count);

    if (ret > 0) fdesc->fpos += ret;

    return ret;
}

int FSUAPI::lseek(int fd, off_t offset, int whence)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;

    int type;
    int ret = FS_CALL(fdesc->node, type)(fdesc->node, &type);
    if (ret < 0) return ret;
    if (type & (S_IFIFO | S_IFSOCK)) return -ESPIPE;

    int64_t size;
    ret = FS_CALL(fdesc->node, size)(fdesc->node, &size);
    if (ret < 0) return ret;

    switch (whence){
        case SEEK_SET:
            if ((int64_t) offset > size || offset < 0) return -EINVAL;
            fdesc->fpos = offset;
            break;

        case SEEK_CUR:
            if ((int64_t) fdesc->fpos + offset > size || fdesc->fpos + offset < 0) return -EINVAL;
		    fdesc->fpos += offset;
            break;

        case SEEK_END:
            if ((int64_t) size + offset > size) return -EINVAL;
            fdesc->fpos = size + offset;
            break;

        default:
            return -EINVAL;
    }

	return fdesc->fpos;
}

int FSUAPI::fsync(int fd)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, fsync)(fdesc->node);
}

int FSUAPI::fdatasync(int fd)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, fdatasync)(fdesc->node);
}

//NOTE: 64 bit implementation
int FSUAPI::truncate(userptr const char *path, uint64_t length)
{
    UserString fPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node);

	if (result >= 0) result = FS_CALL(node, truncate)(node, length);

	return result;
}

//NOTE: 64 bit implementation
int FSUAPI::ftruncate(int fd, uint64_t length)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, truncate)(fdesc->node, length);
}

int FSUAPI::chmod(userptr const char *path, mode_t mode)
{
    UserString fPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node);

	if (result >= 0) result = FS_CALL(node, chmod)(node, mode);

	return result;
}

int FSUAPI::fchmod(int fildes, mode_t mode)
{
        CHECK_FOR_EBADF(fildes);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fildes);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, chmod)(fdesc->node, mode);
}

int FSUAPI::chown(userptr const char *path, uid_t owner, gid_t group)
{
    UserString fPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node);

	if (result >= 0) result = FS_CALL(node, chown)(node, owner, group);

	return result;
}

int FSUAPI::fchown(int fd, uid_t owner, gid_t group)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, chown)(fdesc->node, owner, group);
}

int FSUAPI::lchown(userptr const char *path, uid_t owner, gid_t group)
{
    UserString fPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

	VNode *node;

	int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &node, false);

	if (result >= 0) result = FS_CALL(node, chown)(node, owner, group);

	return result;
}

int FSUAPI::pread(int fd, void *buf, size_t count, uint64_t offset)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, read)(fdesc->node, offset, (char *) buf, count);
}

int FSUAPI::pwrite(int fd, const void *buf, size_t count, uint64_t offset)
{
        CHECK_FOR_EBADF(fd);
	FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
	if (fdesc == NULL) return -EBADF;

	return FS_CALL(fdesc->node, write)(fdesc->node, offset, (char *) buf, count);
}

//TODO
int FSUAPI::fcntl(int fd, int cmd, long arg)
{
	return -EINVAL;
}

int FSUAPI::ioctl(int d, int request, long arg)
{
    CHECK_FOR_EBADF(d);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(d);
    if (fdesc == NULL) return -EBADF;

    int retVal = FS_CALL(fdesc->node, ioctl)(fdesc->node, request, arg);
    if (retVal == -EIOCTLNOTSUPPORTED){
        switch (request){
            case TCGETS: {
                return -ENOTTY;
            }
            default: {
                return -EINVAL;
            }
        }
    }

    return retVal;
}

int FSUAPI::pathToParentAndName(const char *pathname, VNode **parentDirectory, char **name)
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

int FSUAPI::creat(const char *pathname, mode_t mode)
{
    VNode *node;
    
    createNewFile(pathname, mode, &node);
    return open(pathname, 0);
}


int FSUAPI::utime(userptr const char *filename, userptr const struct utimbuf *buf)
{
    UserString fPath(filename, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *tmpnode;
    int result = VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fPath.data(), &tmpnode, false);
    if (result < 0) {
        return result;
    }

    struct utimbuf utim;
    int copyRet = memcpyFromUser(&utim, buf, sizeof(struct utimbuf));
    if (UNLIKELY(copyRet < 0)) {
        return copyRet;
    }
    result = FS_CALL(tmpnode, utime)(tmpnode, &utim);

    VNodeManager::PutVnode(tmpnode);

    return result;
}

int FSUAPI::statfs(userptr const char *path, userptr struct statfs *buf)
{
    UserString fsPath(path, MAX_FILENAME_LEN);
    if (UNLIKELY(!fsPath.isValid())) {
        return fsPath.errorCode();
    }

    VNode *tmpnode;
    int result = VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, fsPath.data(), &tmpnode, false);
    if (result < 0) return result;

    struct statfs s;
    result = FS_CALL(tmpnode, statfs)(tmpnode, &s);
    VNodeManager::PutVnode(tmpnode);
    if (UNLIKELY(result < 0)) {
        return result;
    }
    int ret = memcpyToUser(buf, &s, sizeof(struct statfs));
    if (UNLIKELY(ret < 0)) {
        return ret;
    }

    return result;
}

int FSUAPI::fstatfs(int fd, userptr struct statfs *buf)
{
    CHECK_FOR_EBADF(fd);
    FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
    if (fdesc == NULL) return -EBADF;

    struct statfs s;
    int statFSRet = FS_CALL(fdesc->node, statfs)(fdesc->node, &s);
    if (UNLIKELY(statFSRet < 0)) {
        return statFSRet;
    }
    int ret = memcpyToUser(buf, &s, sizeof(struct statfs));
    if (UNLIKELY(ret < 0)) {
        return  ret;
    }

    return statFSRet;
}

int FSUAPI::unlink(userptr const char *pathname)
{
    UserString fPath(pathname, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *node;
    char *name;
    
    int result = pathToParentAndName(fPath.data(), &node, &name);
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

int FSUAPI::link(userptr const char *oldpath, userptr const char *newpath)
{
    UserString oldPath(oldpath, MAX_FILENAME_LEN);
    if (UNLIKELY(!oldPath.isValid())) {
        return oldPath.errorCode();
    }

    UserString newPath(newpath, MAX_FILENAME_LEN);
    if (UNLIKELY(!newPath.isValid())) {
        return newPath.errorCode();
    }

    VNode *oldNode;
    int result = FileSystem::VFS::RelativePathToVnode(Scheduler::currentThread()->parentProcess->currentWorkingDirNode, oldPath.data(), &oldNode, false);
    if (result < 0){
        return result;
    }
    
    VNode *node;
    char *name;
    result = pathToParentAndName(newPath.data(), &node, &name);
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

int FSUAPI::symlink(userptr const char *oldpath, userptr const char *newpath)
{
    UserString oldPath(oldpath, MAX_FILENAME_LEN);
    if (UNLIKELY(!oldPath.isValid())) {
        return oldPath.errorCode();
    }

    UserString newPath(newpath, MAX_FILENAME_LEN);
    if (UNLIKELY(!newPath.isValid())) {
        return newPath.errorCode();
    }

    VNode *node;
    char *name;
    
    int result = pathToParentAndName(newPath.data(), &node, &name);
    if (result < 0){
      return result;
    }
    
    result = FS_CALL(node, symlink)(node, oldPath.data(), name);
    
    VNodeManager::PutVnode(node);
    free(name);
    
    return result;  
}

int FSUAPI::mknod(userptr const char *pathname, mode_t mode, dev_t dev)
{
    UserString fPath(pathname, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *node;
    char *name;
    
    int result = pathToParentAndName(fPath.data(), &node, &name);
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

int FSUAPI::mkdir(userptr const char *pathname, mode_t mode)
{
    UserString fPath(pathname, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *node;
    char *name;

    int result = pathToParentAndName(fPath.data(), &node, &name);
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

int FSUAPI::rmdir(userptr const char *pathname)
{
    UserString fPath(pathname, MAX_FILENAME_LEN);
    if (UNLIKELY(!fPath.isValid())) {
        return fPath.errorCode();
    }

    VNode *node;
    char *name;
    
    int result = pathToParentAndName(fPath.data(), &node, &name);
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

int FSUAPI::rename(userptr const char *oldpath, userptr const char *newpath)
{
    UserString oldPath(oldpath, MAX_FILENAME_LEN);
    if (UNLIKELY(!oldPath.isValid())) {
        return oldPath.errorCode();
    }

    UserString newPath(newpath, MAX_FILENAME_LEN);
    if (UNLIKELY(!newPath.isValid())) {
        return newPath.errorCode();
    }

    VNode *oldNode;
    char *oldName;
    int result = pathToParentAndName(oldPath.data(), &oldNode, &oldName);
    if (result < 0){
      return result;
    }
    result = isValidUserFileName(oldName);
    if (result < 0){
        return result;
    }

    VNode *newNode;
    char *newName;
    result = pathToParentAndName(newPath.data(), &newNode, &newName);
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

int FSUAPI::dup(int oldfd)
{
    CHECK_FOR_EBADF(oldfd);
    FileDescriptor *oldFdesc = Scheduler::currentThread()->parentProcess->openFiles->at(oldfd);
    if (oldFdesc == NULL) return -EBADF;
    
    FileDescriptor *fdesc = new FileDescriptor(VNodeManager::ReferenceVnode(oldFdesc->node));
    fdesc->flags = oldFdesc->flags;
    fdesc->fpos = oldFdesc->fpos;
    
    return Scheduler::currentThread()->parentProcess->openFiles->add(fdesc);
}

int FSUAPI::dup2(int oldfd, int newfd)
{
    if (newfd == oldfd){
        return newfd;

    }else{
        return dup3(oldfd, newfd, 0);
    }
}

int FSUAPI::dup3(int oldfd, int newfd, int flags)
{
    if (newfd < 0) return -EBADF;
    CHECK_FOR_EBADF(oldfd);
    FileDescriptor *oldFdesc = Scheduler::currentThread()->parentProcess->openFiles->at(oldfd);
    if (oldFdesc == NULL) return -EBADF;
    if (newfd == oldfd) return -EINVAL;

    if (newfd < Scheduler::currentThread()->parentProcess->openFiles->size()){
        FileDescriptor *closefdesc = Scheduler::currentThread()->parentProcess->openFiles->at(newfd);
        if (closefdesc != NULL){
            close(newfd);
        }
    }else{
        Scheduler::currentThread()->parentProcess->openFiles->resize(newfd);
    }

    FileDescriptor *fdesc = new FileDescriptor(VNodeManager::ReferenceVnode(oldFdesc->node));
    fdesc->flags = oldFdesc->flags | flags;
    fdesc->fpos = oldFdesc->fpos;
    (*Scheduler::currentThread()->parentProcess->openFiles)[newfd] = fdesc;
    return newfd;
}

//TODO: check pipefd address
int FSUAPI::pipe2(int pipefd[2], int flags)
{
    VNode *node = Pipe::newPipe();
    if (node == NULL) return -ENOMEM;

    //read end
    FileDescriptor *fdesc0 = new FileDescriptor(node);
    fdesc0->flags = O_RDONLY | flags;
    pipefd[0] = Scheduler::currentThread()->parentProcess->openFiles->add(fdesc0);

    //write end
    FileDescriptor *fdesc1 = new FileDescriptor(VNodeManager::ReferenceVnode(node));
    fdesc1->flags = O_WRONLY | flags;
    pipefd[1] = Scheduler::currentThread()->parentProcess->openFiles->add(fdesc1);

    return 0;
}

int FSUAPI::pipe(int pipefd[2])
{
    return pipe2(pipefd, 0);
}

/*
 How I should do this
 - Disable scheduling
 - Set thread as IWaiting
 - Set all event listeners
 - Check if some IO is already waiting
 - Schedule
*/
int FSUAPI::poll(pollfd *fds, int nfds, int timeout)
{
    if (timeout == 0) return 0;

    Scheduler::inhibitPreemption();
    Scheduler::currentThread()->status = IWaiting;

    for (int i = 0; i < nfds; i++){
        int fd = fds[i].fd;
        if (!((fd >= 0) && (fd < Scheduler::currentThread()->parentProcess->openFiles->size()))) continue;
        FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
        if (fdesc == NULL) continue;

        EventsManager::EventType eType = EventsManager::NoEvent;
        if (fds[0].events & POLLIN) eType = (EventsManager::EventType) (eType | EventsManager::NewDataAvail);

        //TODO: check what we are polling for
        EventsManager::connectEventListener(fdesc->node, Scheduler::currentThread(), eType);
    }

    schedule();

    for (int i = 0; i < nfds; i++){
        int fd = fds[i].fd;
        if (!((fd >= 0) && (fd < Scheduler::currentThread()->parentProcess->openFiles->size()))){
            fds[i].revents = POLLNVAL;
            continue;
        }
        FileDescriptor *fdesc = Scheduler::currentThread()->parentProcess->openFiles->at(fd);
        if (fdesc == NULL){
            fds[i].revents = POLLNVAL;
            continue;
        }

        fds[i].revents = 0;
        if (! EventsManager::disconnectEventListener(fdesc->node, Scheduler::currentThread())){
           fds[i].revents |= POLLIN; //TODO: we are assuming POLLIN
        }
    }

    return 0;
}

int FSUAPI::socket(int domain, int type, int protocol)
{
    VNode *node = Socket::newSocket(domain, type, protocol);
    if (node == NULL) return -ENOMEM;

    FileDescriptor *fdesc = new FileDescriptor(node);
    fdesc->flags = O_RDWR;
    return Scheduler::currentThread()->parentProcess->openFiles->add(fdesc);
}

