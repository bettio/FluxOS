/***************************************************************************
 *   Copyright 2012 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: pipe.cpp                                                        *
 ***************************************************************************/

#include <filesystem/pipe.h>

#include <core/printk.h>

#include <filesystem/vfs.h>
#include <filesystem/vnodemanager.h>

using namespace FileSystem;

unsigned long long Pipe::pipesCounter;
FSMount *Pipe::pipeFakeMount;

struct PipeDesc
{
    unsigned int rpos;
    unsigned int wpos;
    unsigned int count;
    char *buf;
};

int Pipe::init()
{
    pipeFakeMount = new FSMount;
    pipeFakeMount->fs = NewFSModuleInfo(); 
 
    return 0;
}

VNode *Pipe::newPipe()
{
    VNode *vnd;
    VNodeManager::GetVnode(PIPE_MOUNTID, pipesCounter, &vnd);
    if (vnd == NULL) return NULL;
    vnd->mount = pipeFakeMount;

    PipeDesc *pd = new PipeDesc;
    char *buf = (char *) malloc(PIPE_SIZE);
    if (pd == NULL || buf == NULL){
        VNodeManager::PutVnode(vnd);
        return NULL;
    }

    pd->rpos = 0;
    pd->wpos = 0;
    pd->count = 0;
    pd->buf = buf;
    vnd->privdata = pd;
    
    pipesCounter++;
    
    return vnd;
}

FSModuleInfo *Pipe::NewFSModuleInfo()
{
    FSModuleInfo *info = new FSModuleInfo;
    if (info == NULL) return 0;

    info->umount = Umount;
    info->openfd = OpenFD;
    info->closefd = CloseFD;
    info->dupfd = DupFD;
    info->lookup = Lookup;
    info->closevnode = CloseVNode;
    info->read = Read;
    info->readlink = Readlink;
    info->write = write;
    info->getdents = GetDEnts;
    info->stat = Stat;
    info->size = Size;
    info->type = Type;
    info->name = Name;
    info->access = Access;
    info->chmod = Chmod;
    info->chown = Chown;
    info->link = Link;
    info->symlink = Symlink;
    info->rename = Rename;
    info->mknod = Mknod;
    info->mkdir = Mkdir;
    info->truncate = Truncate;
    info->fsync = FSync;
    info->fdatasync = FDataSync;
    info->unlink = Unlink;
    info->rmdir = Rmdir;
    info->creat = Creat;
    info->statfs = StatFS;
    info->utime = Utime;
    info->fcntl = Fcntl;
    info->ioctl = Ioctl;
    info->mmap = Mmap;
    
    return info;
}

int Pipe::Umount(VNode *root)
{
    return -EINVAL;
}

int Pipe::OpenFD(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int Pipe::CloseFD(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int Pipe::DupFD(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int Pipe::Lookup(VNode *node, const char *name, VNode **vnd, unsigned int *ntype)
{
    return -EINVAL;
}

int Pipe::CloseVNode(VNode *node)
{
    PipeDesc *pd = (PipeDesc *) node->privdata;
    if (pd != 0){
        free(pd->buf);
    }
    delete pd;

    return 0;
}

//TODO: if we don't have anything to read we should block (except when O_NONBLOCK is used)
int Pipe::Read(VNode *node, uint64_t pos, char *buf, unsigned int size)
{
    PipeDesc *pd = (PipeDesc *) node->privdata;

    if (pd->count == 0) return 0;

    int readBytes = 0;

    if (pd->rpos >= pd->wpos){
        int rs = (size + pd->rpos > PIPE_SIZE) ? (PIPE_SIZE - pd->rpos) : size;
        memcpy(buf, pd->buf + pd->rpos, rs);
        size -= rs;
        buf += rs;
        readBytes += rs;
        pd->rpos = (pd->rpos + rs) % PIPE_SIZE;
    }

    if (pd->rpos < pd->wpos){
        int rs = (size + pd->rpos > pd->wpos) ? (pd->wpos - pd->rpos) : size;
        memcpy(buf, pd->buf + pd->rpos, rs);
        pd->rpos = (pd->rpos + rs) % PIPE_SIZE;
        readBytes += rs;
    }

    pd->count -= readBytes;
    return readBytes;
}

int Pipe::Readlink(VNode *node, char *buffer, size_t bufsize)
{
    return -EINVAL;
}

//TODO: if we don't have space we should block (except when O_NONBLOCK is used)
int Pipe::write(VNode *node, uint64_t pos, const char *buf, unsigned int size, WriteOpFlags flags)
{
    PipeDesc *pd = (PipeDesc *) node->privdata;

    if (pd->count == PIPE_SIZE) return 0;

    int wBytes = 0;

    if (pd->wpos >= pd->rpos){
        int ws = (size + pd->wpos > PIPE_SIZE) ? PIPE_SIZE - pd->wpos : size;
        memcpy(pd->buf + pd->wpos, buf, ws);
        size -= ws;
        buf += ws;
        wBytes += ws;
        pd->wpos = (pd->wpos + ws) % PIPE_SIZE;
    }

    if (pd->wpos < pd->rpos){
        int ws = (size + pd->wpos > pd->rpos) ? pd->rpos - pd->wpos : size;
        memcpy(pd->buf + pd->wpos, buf, ws);
        pd->wpos = (pd->wpos + ws) % PIPE_SIZE;
        wBytes += ws;
    }

    pd->count += wBytes;
    return wBytes;
}

int Pipe::GetDEnts(VNode *node, dirent *dirp, unsigned int count)
{
    return -EINVAL;
}

int Pipe::Stat(VNode *node, struct stat *buf)
{
    return -EINVAL;
}

int Pipe::Access(VNode *node, int aMode, int uid, int gid)
{
    return -EINVAL;
}

int Pipe::Chmod(VNode *node, mode_t mode)
{
    return -EINVAL;
}

int Pipe::Chown(VNode *node, uid_t uid, gid_t gid)
{
    return -EINVAL;
}

int Pipe::Link(VNode *directory, VNode *oldNode, const char *newName)
{
    return -EINVAL;
}

int Pipe::Symlink(VNode *directory, const char *oldName, const char *newName)
{
   
    return -EINVAL;
}

int Pipe::Rename(VNode *oldDirectory, const char *oldName, VNode *newDirectory, const char *newName)
{
    return -EINVAL;
}

int Pipe::Mknod(VNode *directory, const char *newName, mode_t mode, dev_t dev)
{
    return -EINVAL;
}

int Pipe::Mkdir(VNode *directory, const char *newName, mode_t mode)
{
    return -EINVAL;
}

int Pipe::Truncate(VNode *node, uint64_t length)
{
    return -EINVAL;
}

int Pipe::FSync(VNode *node)
{
    return -EINVAL;
}

int Pipe::FDataSync(VNode *node)
{
    return -EINVAL;
}

int Pipe::Unlink(VNode *directory, const char *name)
{
    return -EINVAL;
}

int Pipe::Rmdir(VNode *directory, const char *name)
{
    return -EINVAL;
}

int Pipe::Creat(VNode *directory, const char *name, mode_t mode)
{
    return -EINVAL;
}

int Pipe::Name(VNode *directory, VNode *node, char **name, int *len)
{
   return -EINVAL;
}

int Pipe::StatFS(VNode *directory, struct statfs *buf)
{  
    return -EINVAL;
}

int Pipe::Size(VNode *node, int64_t *size)
{  
    return -EINVAL;
}

int Pipe::Type(VNode *node, int *type)
{
    *type = S_IFIFO;
    return 0;
}

int Pipe::Utime(VNode *node, const struct utimbuf *buf)
{
    return -EINVAL;
}

int Pipe::Fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int Pipe::Ioctl(VNode *node, int request, long arg)
{
    return -EIOCTLNOTSUPPORTED;
}

void *Pipe::Mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return (void *) -EINVAL;
}
