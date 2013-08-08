/***************************************************************************
 *   Copyright 2013 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: tcpsocketcalls.cpp                                              *
 ***************************************************************************/


#include <net/tcpsocketcalls.h>

#include <core/printk.h>
#include <filesystem/vfs.h>
#include <filesystem/socket.h>
#include <filesystem/vnodemanager.h>
#include <net/ipsocketcalls.h>
#include <net/ip6socketcalls.h>
#include <net/nettypes.h>
#include <net/net.h>
#include <net/route.h>
#include <net/tcp.h>
#include <net/tcpsocket.h>
#include <task/scheduler.h>
#include <task/eventsmanager.h>

using namespace FileSystem;

SOCKET_FSCALLS(TCPSocketCalls::calls);

FSMount TCPSocketCalls::tcpSocketFakeMount =
{
    &calls,
    0,
    0,
    0,
    0
};

int TCPSocketCalls::bindToSocket(VNode *node, int domain, int type, int protocol)
{
    TCPSocket *tcpSock;
    if (node->privdata == NULL){
        tcpSock = new TCPSocket;
        tcpSock->localAddr = NULL;
        tcpSock->remoteAddr = NULL;
        tcpSock->queuedConnections = NULL;

        node->mount = &tcpSocketFakeMount;
        node->privdata = tcpSock;
    }

    return 0;
}

int TCPSocketCalls::openfd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int TCPSocketCalls::closefd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int TCPSocketCalls::dupfd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int TCPSocketCalls::closevnode(VNode *node)
{
    return 0;
}

int TCPSocketCalls::accept(VNode *socknode, struct sockaddr *addr, int *addrlen, VNode **newEndPoint)
{
    TCPSocket *tcpSock = (TCPSocket *) socknode->privdata;
    if (tcpSock->queuedConnections == NULL) return -EINVAL;

    if (tcpSock->queuedConnections->count()){
        *newEndPoint = Socket::newSocketEndPoint(tcpSock->queuedConnections->takeFirst());
        if (newEndPoint == NULL){
            return -ENOMEM;
        }else{
            return 0;
        }
    }

    Scheduler::inhibitPreemption();
    Scheduler::currentThread()->status = IWaiting;
    EventsManager::connectEventListener(socknode, Scheduler::currentThread(), EventsManager::NewDataAvail);
    schedule();

    if (tcpSock->queuedConnections->count()){
        *newEndPoint = Socket::newSocketEndPoint(tcpSock->queuedConnections->takeFirst());
        if (newEndPoint == NULL){
            return -ENOMEM;
        }else{
            return 0;
        }
    }

    return 0;
}

int TCPSocketCalls::bind(VNode *socknode, const struct sockaddr *addr, int addrlen)
{
     TCPSocket *tcpSock = (TCPSocket *) socknode->privdata;
     tcpSock->localAddr = (sockaddr *) malloc(addrlen);
     if (tcpSock->localAddr == NULL){
         return -ENOMEM;
     }
     memcpy(tcpSock->localAddr, addr, addrlen);
     tcpSock->datagrams = new QList<void *>();
     if (tcpSock->datagrams == NULL){
         free(tcpSock->localAddr);
         return -ENOMEM;
     }

     return TCP::bindVNodeToPort(((const sockaddr_in *) addr)->sin_port, socknode);
}
        
int TCPSocketCalls::connect(VNode *socknode, const struct sockaddr *addr, int addrlen)
{
     TCPSocket *udpSock = (TCPSocket *) socknode->privdata;
     if (udpSock->remoteAddr != NULL) free(udpSock->remoteAddr);
     udpSock->remoteAddr = (sockaddr *) malloc(addrlen);
     if (udpSock->remoteAddr == NULL){
         return -ENOMEM;
     }
     memcpy(udpSock->remoteAddr, addr, addrlen);

     return 0;
}

int TCPSocketCalls::getpeername(VNode *socknode, struct sockaddr *addr, int *addrlen)
{
     TCPSocket *udpSock = (TCPSocket *) socknode->privdata;
     if (udpSock->remoteAddr == NULL) return -ENOTCONN;

     int len = MIN(*addrlen, (int) sizeof(sockaddr_in));
     memcpy(addr, udpSock->remoteAddr, len); //FIXME: support for ipv6
     return 0;
}

int TCPSocketCalls::getsockname(VNode *socknode, struct sockaddr *addr, int *addrlen)
{
     TCPSocket *udpSock = (TCPSocket *) socknode->privdata;

     int len = MIN(*addrlen, (int) sizeof(sockaddr_in));
     memcpy(addr, udpSock->localAddr, len); //FIXME: support for ipv6
     return 0;
}

int TCPSocketCalls::getsockopt(VNode *socknode, int level, int optname, void *optval, int *optlen)
{
     return -EINVAL;
}

int TCPSocketCalls::listen(VNode *socknode, int backlog)
{
    TCPSocket *tcpSock = (TCPSocket *) socknode->privdata;
    tcpSock->queuedConnections = new QList<TCPSocket *>;
    if (tcpSock->queuedConnections == NULL) return -ENOMEM;
    if (!tcpSock->queuedConnections->reserve(backlog)) return -ENOMEM;
}

int TCPSocketCalls::recv(VNode *socknode, void *buf, size_t len, int flags)
{

     return recvfrom(socknode, buf, len, flags, NULL, NULL);
}

int TCPSocketCalls::recvfrom(VNode *socknode, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen)
{
#if 0
     TCPSocket *udpSock = (TCPSocket *) socknode->privdata;

     if (udpSock->datagrams->count()){
         void *datagram = udpSock->datagrams->takeFirst();
         const TCPHeader *header = (const TCPHeader *) datagram;
         const void *data = (((const uint8_t *) (datagram)) + sizeof(TCPHeader));
         int readData = MIN(len, header->length - sizeof(TCPHeader));
         memcpy(buf, data, readData);
         free(datagram);
         return readData;
     }

     Scheduler::inhibitPreemption();
     Scheduler::currentThread()->status = IWaiting;
     EventsManager::connectEventListener(socknode, Scheduler::currentThread(), EventsManager::NewDataAvail);
     schedule();

     if (udpSock->datagrams->count()){
         void *datagram = udpSock->datagrams->takeFirst();
         const TCPHeader *header = (const TCPHeader *) datagram;
         const void *data = (((const uint8_t *) (datagram)) + sizeof(TCPHeader));
         int readData = MIN(len, header->length - sizeof(TCPHeader));
         memcpy(buf, data, readData);
         free(datagram);
         return readData;
     }

     return 0;
#endif
}

int TCPSocketCalls::recvmsg(VNode *socknode, struct msghdr *msg, int flags)
{
     return 0;
}

int TCPSocketCalls::send(VNode *socknode, const void *buf, size_t len, int flags)
{
     TCPSocket *udpSock = (TCPSocket *) socknode->privdata;
     if (udpSock->remoteAddr == NULL){
         return -ENOTCONN;
     }

     NetIface *iface = Net::interface("eth0");

     ipaddr dest;
     dest.addr = ((sockaddr_in *) udpSock->remoteAddr)->sin_addr.s_addr;
    
     //TCP::sendTo(iface, iface->myIP, dest, ((sockaddr_in *) udpSock->localAddr)->sin_port, ((sockaddr_in *) udpSock->remoteAddr)->sin_port, (uint8_t *) buf, len); 

     return len;
}

int TCPSocketCalls::sendmsg(VNode *socknode, const struct msghdr *msg, int flags)
{
     return 0;
}


int TCPSocketCalls::sendto(VNode *socknode, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, int addrlen)
{
     TCPSocket *udpSock = (TCPSocket *) socknode->privdata;
     NetIface *iface = Net::interface("eth0");

     ipaddr dest;
     dest.addr = ((sockaddr_in *) dest_addr)->sin_addr.s_addr;
     //TCP::sendTo(iface, iface->myIP, dest, ((sockaddr_in *) udpSock->localAddr)->sin_port, ((sockaddr_in *) dest_addr)->sin_port, (uint8_t *) buf, len); 

     return len;
}

int TCPSocketCalls::setsockopt(VNode *socknode, int level, int optname, const void *optval, int optlen)
{
     return -EINVAL;
}

int TCPSocketCalls::shutdown(VNode *socknode, int how)
{
     return 0;
}

int TCPSocketCalls::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
     return recv(node, buffer, bufsize, 0);
}

int TCPSocketCalls::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
     return send(node, buffer, bufsize, 0);
}

int TCPSocketCalls::type(VNode *node, int *type)
{
    *type = S_IFSOCK;
    return 0;
}

int TCPSocketCalls::fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int TCPSocketCalls::ioctl(VNode *node, int request, long arg)
{
    return IPSocketCalls::ioctl(node, request, arg);
}

