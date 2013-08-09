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
 *   Name: socket.cpp                                                      *
 ***************************************************************************/

#include <core/printk.h>

#include <filesystem/vfs.h>
#include <filesystem/vnodemanager.h>
#include <net/ipsocketcalls.h>
#include <net/nettypes.h>
#include <net/net.h>
#include <net/route.h>

using namespace FileSystem;

SOCKET_FSCALLS(IPSocketCalls::calls);

FSMount IPSocketCalls::ipSocketFakeMount =
{
    &calls,
    0,
    0,
    0,
    0
};

int IPSocketCalls::bindToSocket(VNode *node, int domain, int type, int protocol)
{
    if (node->privdata == NULL){
        node->mount = &ipSocketFakeMount;
    }

    return 0;
}

int IPSocketCalls::openfd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int IPSocketCalls::closefd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int IPSocketCalls::dupfd(VNode *node, FileDescriptor *fdesc)
{
    return 0;
}

int IPSocketCalls::closevnode(VNode *node)
{
    return 0;
}

int IPSocketCalls::accept(VNode *socknode, struct sockaddr *addr, int *addrlen, VNode **newEndPoint)
{
     return -EOPNOTSUPP;
}

int IPSocketCalls::bind(VNode *socknode, const struct sockaddr *addr, int addrlen)
{
     return 0;
}
        
int IPSocketCalls::connect(VNode *socknode, const struct sockaddr *addr, int addrlen)
{
     return 0;
}

int IPSocketCalls::getpeername(VNode *socknode, struct sockaddr *addr, int *addrlen)
{
     return 0;
}

int IPSocketCalls::getsockname(VNode *socknode, struct sockaddr *addr, int *addrlen)
{
     return 0;
}

int IPSocketCalls::getsockopt(VNode *socknode, int level, int optname, void *optval, int *optlen)
{
     return 0;
}

int IPSocketCalls::listen(VNode *socknode, int backlog)
{
     return 0;
}

int IPSocketCalls::recv(VNode *socknode, void *buf, size_t len, int flags)
{
     return 0;
}

int IPSocketCalls::recvfrom(VNode *socknode, void *buf, size_t len, int flags, struct sockaddr *src_addr, int *addrlen)
{
     return 0;
}

int IPSocketCalls::recvmsg(VNode *socknode, struct msghdr *msg, int flags)
{
     return 0;
}

int IPSocketCalls::send(VNode *socknode, const void *buf, size_t len, int flags)
{
     return 0;
}

int IPSocketCalls::sendmsg(VNode *socknode, const struct msghdr *msg, int flags)
{
     return 0;
}


int IPSocketCalls::sendto(VNode *socknode, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, int addrlen)
{
     return 0;
}

int IPSocketCalls::setsockopt(VNode *socknode, int level, int optname, const void *optval, int optlen)
{
     return 0;
}

int IPSocketCalls::shutdown(VNode *socknode, int how)
{
     return 0;
}

int IPSocketCalls::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
     return 0;
}

int IPSocketCalls::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
     return 0;
}

int IPSocketCalls::type(VNode *node, int *type)
{
    *type = S_IFSOCK;
    return 0;
}

int IPSocketCalls::fcntl(VNode *node, int cmd, long arg)
{
    return -EINVAL;
}

int IPSocketCalls::ioctl(VNode *node, int request, long arg)
{
    switch (request){
        case SIOCADDRT: {
            rtentry *rt = (rtentry *) arg;
            NetIface *iface = Net::interface(rt->rt_dev);
            if (iface == NULL){
                return -ENODEV;
            } 
            if ((rt->rt_dst.sa_family == PF_INET) && (rt->rt_gateway.sa_family == PF_INET) && (rt->rt_genmask.sa_family == PF_INET)){
                ipaddr dest;
                dest.addr = ((sockaddr_in *) &rt->rt_dst)->sin_addr.s_addr;
                ipaddr gw;
                gw.addr = ((sockaddr_in *) &rt->rt_gateway)->sin_addr.s_addr;
                ipaddr mask;
                mask.addr = ((sockaddr_in *) &rt->rt_genmask)->sin_addr.s_addr;
                IP::addRoute(dest, mask, gw, iface);
                printk("Added route.\n");
            }

            break;
        }

        case SIOCGIFNAME:
            break;

        case SIOCGIFCONF:
            break;

        case SIOCGIFFLAGS:{
            ifreq *ifr = (ifreq *) arg;
            ifr->ifr_flags = 0;
            break;
        }

        case SIOCSIFFLAGS:
            break;

        case SIOCGIFADDR: {
           break;
        }

        case SIOCSIFADDR: {
            ifreq *ifr = (ifreq *) arg;
            NetIface *iface = Net::interface(ifr->ifr_name);
            if (iface == NULL){
                return -ENODEV;
            }
            if (ifr->ifr_addr.sa_family == PF_INET){
                sockaddr_in *iaddr = (sockaddr_in *) &ifr->ifr_addr;
                iface->myIP.addr = iaddr->sin_addr.s_addr;
                IP::addAddressToInterface(iface, iface->myIP);
                printk("Changed ip address.\n");
            }
            break;
        }

        case SIOCGIFNETMASK: 
            break;

        case SIOCSIFNETMASK:
           break;

        default:
            return -EIOCTLNOTSUPPORTED;
    }

    return 0;
}

