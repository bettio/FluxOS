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
 *   Name: fsuapi.h                                                        *
 *   Date: 01/07/2006                                                      *
 ***************************************************************************/

#ifndef _FILESYSTEM_FSUAPI_H_
#define _FILESYSTEM_FSUAPI_H_

#include <filesystem/vfs.h>
#include <mm/usermemoryops.h>

class FileDescriptor;
class ProcessControlBlock;
struct pollfd;
struct statfs;

class FSUAPI
{
    private:
        static int isValidUserFileName(const char *name);
        static int isValidFileName(const char *name);
        static int createNewFile(const char *pathname, mode_t mode, VNode **node);
        static int openAtVNode(VNode *dirNode, userptr const char *pathname, int flags);

public:
        static int closePB(ProcessControlBlock *process, int fd);

        static int pathToParentAndName(const char *pathname, VNode **parentDirectory, char **name);
        static int creat(userptr const char *pathname, mode_t mode);
        static int utime(userptr const char *path, userptr const struct utimbuf *times);
        static int readlink(userptr const char *path, userptr char *buf, size_t bufsiz);
        static mode_t umask(mode_t mode);
        static int open(userptr const char *pathname, int flags);
        static int openat(int dirfd, userptr const char *pathname, int flags);
        static int close(int fd);
        static int lseek(int fd, off_t offset, int whence);
        static int getdents(int fd, userptr dirent *dirp, unsigned int count);
        static int getcwd(userptr char *buf, size_t size);
        static int chdir(userptr const char *path);
        static int fchdir(int fd);
        static int read(int fd, userptr void *buf, size_t count);
        static int write(int fd, userptr const void *buf, size_t count);
        static int stat(userptr const char *path, struct stat *buf);
        static int fstat(int filedes, userptr struct stat *buf);
        static int lstat(userptr const char *path, struct stat *buf);
        static int stat64(userptr const char *path, struct stat64 *buf);
        static int fstat64(int filedes, userptr struct stat64 *buf);
        static int lstat64(userptr const char *path, struct stat64 *buf);
        static int access(userptr const char *pathname, int mode);

        static int fsync(int fd);
        static int fdatasync(int fd);
        static int truncate(userptr const char *path, uint64_t length);
        static int ftruncate(int fd, uint64_t length);

        static int chmod(userptr const char *path, mode_t mode);
        static int fchmod(int fildes, mode_t mode);
        static int chown(userptr const char *path, uid_t owner, gid_t group);
        static int fchown(int fd, uid_t owner, gid_t group);
        static int lchown(userptr const char *path, uid_t owner, gid_t group);


        static int pread(int fd, userptr void *buf, size_t count, uint64_t offset);
        static int pwrite(int fd, userptr const void *buf, size_t count, uint64_t offset);

        static int fcntl(int fd, int cmd, long arg);
        static int ioctl(int d, int request, long arg);

        static int mkdir(userptr const char *pathname, mode_t mode);
        static int rmdir(userptr const char *pathname);
        static int statfs(userptr const char *path, userptr struct statfs *buf);
        static int fstatfs(int fd, userptr struct statfs *buf);
        static int unlink(userptr const char *pathname);
        static int symlink(userptr const char *oldpath, userptr const char *newpath);
        static int link(userptr const char *oldpath, userptr const char *newpath);
        static int rename(userptr const char *oldpath, userptr const char *newpath);
        static int mknod(userptr const char *pathname, mode_t mode, dev_t dev);

        static int dup(int oldfd);
        static int dup2(int oldfd, int newfd);
        static int dup3(int oldfd, int newfd, int flags);
        static int pipe(int pipefd[2]);
        static int pipe2(int pipefd[2], int flags);

        static int poll(userptr pollfd *fds, int nfds, int timeout);
        static int socket(int domain, int type, int protocol);
};

#endif
