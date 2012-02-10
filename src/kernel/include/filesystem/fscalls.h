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

#ifndef _FILESYSTEM_FSCALLS_H_
#define _FILESYSTEM_FSCALLS_H_

#include <filesystem/vfs.h>

class FileDescriptor;

int pathToParentAndName(const char *pathname, VNode **parentDirectory, char **name);
int creat(const char *pathname, mode_t mode);
int readlink(const char *path, char *buf, size_t bufsiz);
int open(const char *pathname, int flags);
int close(int fd);
int lseek(int fd, off_t offset, int whence);
int getdents(int fd, dirent *dirp, unsigned int count);
int getcwd(char *buf, size_t size);
int chdir(const char *path);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int stat(const char *path, struct stat *buf);
int fstat(int filedes, struct stat *buf);
int lstat(const char *path, struct stat *buf);
int stat64(const char *path, struct stat64 *buf);
int fstat64(int filedes, struct stat64 *buf);
int lstat64(const char *path, struct stat64 *buf);

int fsync(int fd);
int fdatasync(int fd);
int truncate(const char *path, uint64_t length);
int ftruncate(int fd, uint64_t length);

int chmod(const char *path, mode_t mode);
int fchmod(int fildes, mode_t mode);
int chown(const char *path, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int lchown(const char *path, uid_t owner, gid_t group);


int pread(int fd, void *buf, size_t count, uint64_t offset);
int pwrite(int fd, const void *buf, size_t count, uint64_t offset);

int fcntl(int fd, int cmd, long arg);
int ioctl(int d, int request, long arg);

int mkdir(const char *pathname, mode_t mode);
int rmdir(const char *pathname);
int unlink(const char *pathname);
int symlink(const char *oldpath, const char *newpath);
int link(const char *oldpath, const char *newpath);
int rename(const char *oldpath, const char *newpath);
int mknod(const char *pathname, mode_t mode, dev_t dev);

int chmod(const char *path, mode_t mode);
int dup(int oldfd);

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

#endif
