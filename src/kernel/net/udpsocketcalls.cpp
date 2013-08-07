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
 *   Name: udpsocketcalls.cpp                                              *
 ***************************************************************************/


#include <net/udpsocketcalls.h>

#include <core/printk.h>
#include <filesystem/vfs.h>
#include <filesystem/vnodemanager.h>
#include <net/ipsocketcalls.h>
#include <net/ip6socketcalls.h>
#include <net/nettypes.h>
#include <net/net.h>
#include <net/route.h>
#include <net/udp.h>
#include <net/udpsocket.h>
#include <task/scheduler.h>
#include <task/eventsmanager.h>

using namespace FileSystem;

SOCKET_FSCALLS(UDPSocketCalls::calls);

FSMount UDPSocketCalls::udpSocketFakeMount =
{
    &calls,
    0,
    0,
    0,
    0
};

int UDPSocketCalls::bindToSocket(VNode *node, int domain, int type, int protocol)
{
    UDPSocket *udpSock;
    if (node->privdata == NULL){
        udpSock = new UDPSocket;
        udpSock->remoteAddr = NULL;

        node->mount = &udpSocketFakeMount;
        node->privdata = udpSock;
        
    }

    if (domain == PF_INET){
        sockaddr_in *localAddr = (sockaddr_in *) malloc(sizeof(sockaddr_in));
        localAddr->sin_addr.s_addr = INADDR_ANY;
        localAddr->sin_port = 99; //TODO: Random port
        udpSock->localAddr = (sockaddr *) localAddr;

        IPSocketCalls::bindToSocket(node, domain, type, protocol);

    }else if (domain == PF_INET6){
        sockaddr_in6 *localAddr = (sockaddr_in6 *) malloc(sizeof(sockaddr_in6));
        memset(&localAddr->sin6_addr, 0, sizeof(localAddr->sin6_addr)); //IN6ADDR_ANY
        localAddr->sin6_port = 99; //TODO: Random port
        udpSock->localAddr = (sockaddr *) localAddr;

        IP6SocketCalls::bindToSocket(node, domain, type, protocol);
    }

    return 0;
}

int UDPSocketCalls::openfd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int UDPSocketCalls::closefd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int UDPSocketCalls::dupfd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int UDPSocketCalls::closevnode(VNode *node)
{
    return 0;
}

int UDPSocketCalls::accept(VNode *socknode, struct sockaddr *addr, int *addrlen, VNode **newEndPoint)
{
     return -EOPNOTSUPP;
}

int UDPSocketCalls::bind(VNode *socknode, const struct sockaddr *addr, int addrlen)
{
     UDPSocket *udpSock = (UDPSocket *) socknode->privdata;
     free(udpSock->localAddr);
     udpSock->localAddr = (sockaddr *) malloc(addrlen);
     if (udpSock->localAddr == NULL){
         return -ENOMEM;
     }
     memcpy(udpSock->localAddr, addr, addrlen);
     udpSock->datagrams = new QList<void *>();
     if (udpSock->datagrams == NULL){
         free(udpSock->localAddr);
         return -ENOMEM;
     }

     return UDP::bindVNodeToPort(((const sockaddr_in *) addr)->sin_port, socknode);
}
        
int UDPSocketCalls::connect(VNode *socknode, const struct sockaddr *addr, int addrlen)
{
     UDPSocket *udpSock = (UDPSocket *) socknode->privdata;
     if (udpSock->remoteAddr != NULL) free(udpSock->remoteAddr);
     udpSock->remoteAddr = (sockaddr *) malloc(addrlen);
     if (udpSock->remoteAddr == NULL){
         return -ENOMEM;
     }
     memcpy(udpSock->remoteAddr, addr, addrlen);

     return 0;
}

int UDPSocketCalls::getpeername(VNode *socknode, struct sockaddr *addr, int *addrlen)
{
     UDPSocket *udpSock = (UDPSocket *) socknode->privdata;
     if (udpSock->remoteAddr == NULL) return -ENOTCONN;

     int len = MIN(*addrlen, (int) sizeof(sockaddr_in));
     memcpy(addr, udpSock->remoteAddr, len); //FIXME: support for ipv6
     return 0;
}

int UDPSocketCalls::getsockname(VNode *socknode, struct sockaddr *addr, int *addrlen)
{
     UDPSocket *udpSock = (UDPSocket *) socknode->privdata;

     int len = MIN(*addrlen, (int) sizeof(sockaddr_in));
     memcpy(addr, udpSock->localAddr, len); //FIXME: support for ipv6
     return 0;
}

int UDPSocketCalls::getsockopt(VNode *socknode, int level, int optname, void *optval, int *optlen)
{
     return -EINVAL;
}

int UDPSocketCalls::listen(VNode *socknode, int backlog)
{
     return -EOPNOTSUPP;
}

int UDPSocketCalls::recv(VNode *socknode, void *buf, size_t len, int flags)
{

     return recvfrom(socknode, buf, len, flags, NULL, NULL);
}

int UDPSocketCalls::recvfrom(VNode *socknode, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen)
{
     UDPSocket *udpSock = (UDPSocket *) socknode->privdata;

     if (udpSock->datagrams->count()){
         void *datagram = udpSock->datagrams->takeFirst();
         const UDPHeader *header = (const UDPHeader *) datagram;
         const void *data = (((const uint8_t *) (datagram)) + sizeof(UDPHeader));
         int readData = MIN(len, header->length - sizeof(UDPHeader));
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
         const UDPHeader *header = (const UDPHeader *) datagram;
         const void *data = (((const uint8_t *) (datagram)) + sizeof(UDPHeader));
         int readData = MIN(len, header->length - sizeof(UDPHeader));
         memcpy(buf, data, readData);
         free(datagram);
         return readData;
     }

     return 0;
}

int UDPSocketCalls::recvmsg(VNode *socknode, struct msghdr *msg, int flags)
{
     return 0;
}

int UDPSocketCalls::send(VNode *socknode, const void *buf, size_t len, int flags)
{
     UDPSocket *udpSock = (UDPSocket *) socknode->privdata;
     if (udpSock->remoteAddr == NULL){
         return -ENOTCONN;
     }

     NetIface *iface = Net::interface("eth0");

     ipaddr dest;
     dest.addr = ((sockaddr_in *) udpSock->remoteAddr)->sin_addr.s_addr;
    
     UDP::sendTo(iface, iface->myIP, dest, ((sockaddr_in *) udpSock->localAddr)->sin_port, ((sockaddr_in *) udpSock->remoteAddr)->sin_port, (uint8_t *) buf, len); 

     return len;
}

int UDPSocketCalls::sendmsg(VNode *socknode, const struct msghdr *msg, int flags)
{
     return 0;
}


int UDPSocketCalls::sendto(VNode *socknode, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, int addrlen)
{
     UDPSocket *udpSock = (UDPSocket *) socknode->privdata;
     NetIface *iface = Net::interface("eth0");

     ipaddr dest;
     dest.addr = ((sockaddr_in *) dest_addr)->sin_addr.s_addr;
     UDP::sendTo(iface, iface->myIP, dest, ((sockaddr_in *) udpSock->localAddr)->sin_port, ((sockaddr_in *) dest_addr)->sin_port, (uint8_t *) buf, len); 

     return len;
}

int UDPSocketCalls::setsockopt(VNode *socknode, int level, int optname, const void *optval, int optlen)
{
     return -EINVAL;
}

int UDPSocketCalls::shutdown(VNode *socknode, int how)
{
     return 0;
}

int UDPSocketCalls::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
     return recv(node, buffer, bufsize, 0);
}

int UDPSocketCalls::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
     return send(node, buffer, bufsize, 0);
}

int UDPSocketCalls::type(VNode *node, int *type)
{
    *type = S_IFSOCK;
    return 0;
}

int UDPSocketCalls::fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int UDPSocketCalls::ioctl(VNode *node, int request, long arg)
{
    return IPSocketCalls::ioctl(node, request, arg);
}

