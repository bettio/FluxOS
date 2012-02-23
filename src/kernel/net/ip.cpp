/***************************************************************************
 *   Copyright 2009 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: ip.cpp                                                          *
 ***************************************************************************/

#include <net/ip.h>

#include <net/icmp.h>
#include <net/net.h>
#include <net/netutils.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/ethernet.h>
#include <net/arp.h>

#define ENABLE_DEBUG_MSG 1
#include <debugmacros.h>

QList<Route *> *IP::routes;
Route *IP::defaultRoute;
bool IP::ipForwardingEnabled;

void IP::init()
{
    routes = new QList<Route *>();
    defaultRoute = 0;
}

void IP::addRoute(ipaddr dest, ipaddr mask, ipaddr gateway, NetIface *iface)
{
    Route *route = new Route;
    route->dest = dest;
    route->mask = mask;
    route->gateway = gateway;
    route->iface = iface;
    routes->append(route);
}

void IP::addAddressToInterface(NetIface *iface, ipaddr addr)
{
    iface->myIP = addr;
}

void IP::processIPPacket(NetIface *iface, uint8_t *packet, int size)
{
    IPHeader *header = (IPHeader *) packet;

    if ((uint16_t) size < sizeof(IPHeader)){
        DEBUG_MSG("IP: packet length is smaller than min header size. (reported size: %i).\n", size);
        return;
    }
    if (header->version != 4){
        DEBUG_MSG("Not supported IP version.\n");
        return;
    }
    if ((unsigned int) header->ihl*4 < sizeof(IPHeader)){
        DEBUG_MSG("IP: packet header len is smaller than min header size. (size: %i).\n", header->ihl*4);
        return;
    }
    if (ntohs(header->tot_len) > size){
        DEBUG_MSG("IP: discarded packet: wrong total len\n");
        return;
    }
    if (header->ihl*4 > ntohs(header->tot_len)){
        DEBUG_MSG("IP: discarded packet: packet header is bigger than packet\n");
        return;
    }

    if (header->daddr.addr != iface->myIP.addr){
        if (ipForwardingEnabled){
            forwardPacket(packet);
            return;
        }else{
            return;
        }
    }

    if (ntohs(header->frag_off) && !(ntohs(header->frag_off) & IP_FLAGS_DF)){
        processIPFragment(iface, packet, size);
        return;
    }

    switch(header->protocol){
        case PROTOCOL_ICMP:
            DEBUG_MSG("Net: ICMP packet\n");
            ICMP::processICMPPacket(iface, packet + header->ihl*4, ntohs(header->tot_len) - header->ihl*4, header, 4);

            break;

        case PROTOCOL_TCP:
            DEBUG_MSG("Net: TCP packet\n");
            TCP::processTCPPacket(iface, packet + header->ihl*4, ntohs(header->tot_len) - header->ihl*4, header, 4);

            break;

        case PROTOCOL_UDP:
            DEBUG_MSG("Net: UDP packet\n");
            UDP::processUDPPacket(iface, packet + header->ihl*4, ntohs(header->tot_len) - header->ihl*4, header, 4);

            break;

        default:
            DEBUG_MSG("Unknown packet: protocol: %i\n", header->protocol);

            break;
    }
}


void IP::processIPFragment(NetIface *iface, uint8_t *packet, int size)
{
    return;
}

void IP::buildIPHeader(NetIface *iface, uint8_t *buffer, ipaddr destinationIP, uint8_t protocol, uint16_t dataLen)
{
    IPHeader *newIPHeader = (IPHeader *) buffer;
    newIPHeader->version = 4;
    newIPHeader->ihl = 5;
    newIPHeader->tos = 0;
    newIPHeader->tot_len = htons(dataLen);
    newIPHeader->id = 0;
    newIPHeader->frag_off = htons(0x4000);
    newIPHeader->ttl = 64;
    newIPHeader->protocol = protocol;
    newIPHeader->check = 0;
    newIPHeader->saddr = iface->myIP;
    newIPHeader->daddr = destinationIP;
    newIPHeader->check = checksum((uint16_t *) newIPHeader, sizeof(IPHeader));
}


uint16_t IP::upperLayerChecksum(ipaddr saddr, ipaddr daddr, int protocol, void *header, int size)
{
    IPFakeHeader ipFake;
    ipFake.saddr = saddr;
    ipFake.daddr = daddr;
    ipFake.zero = 0;
    ipFake.protocol = protocol;
    ipFake.payloadLen = htons(size);
    return checksum((uint16_t *) &ipFake, sizeof(IPFakeHeader), (uint16_t *) header, size);
}

Route *IP::route(ipaddr destIP)
{
    for (int i = 0; i < routes->size(); i++){
        if ((destIP.addr & routes->at(i)->mask.addr) == routes->at(i)->dest.addr){
            return routes->at(i);
        }
    }
    return defaultRoute; 
}

//TODO: add support for unreachable host
bool IP::route(ipaddr destIP, NetIface **destIf, macaddr *destMac)
{
    Route *rt = route(destIP);
    if (rt == 0){
        DEBUG_MSG("IP: destination unreachable\n");
        return false;
    }
    macaddr macAddr;
    NetIface *iface = rt->iface;
    ipaddr targetIP = (rt->gateway.addr) ? rt->gateway : destIP;
    macAddr = iface->macCache.value(targetIP.addr);
    if (!(macAddr.addrbytes[0] | macAddr.addrbytes[1] | macAddr.addrbytes[2] | macAddr.addrbytes[3])){
        DEBUG_MSG("IP: mac cache miss.\n");
        ARP::sendARPRequest(iface, targetIP);
        //TODO: wait for ARP reply
    }

    *destIf = iface;
    *destMac = macAddr;
    return true;
}

void IP::forwardPacket(uint8_t *packet)
{
    IPHeader *header = (IPHeader *) packet;

    macaddr macAddr;
    NetIface *iface;
    if (!route(header->daddr, &iface, &macAddr)){
        ICMP::sendICMPReply(iface, packet, ntohs(header->tot_len), header->saddr, ICMP_UNREACHABLE, ICMP_UNREACHABLE_NET);
        return;
    }

    int offset;
    uint8_t *newPacket = (uint8_t *) iface->allocPacketFor(iface, packet, ntohs(header->tot_len), macAddr, ETHERTYPE_IP, &offset);
    memcpy(newPacket + offset, packet, ntohs(header->tot_len));
    IPHeader *newIPHeader = (IPHeader *) (newPacket + offset);
    if (newIPHeader->ttl > 1){
        newIPHeader->ttl--;
        newIPHeader->check = 0;
        newIPHeader->check = checksum((uint16_t *) newIPHeader, sizeof(IPHeader));
    }else{
        ICMP::sendICMPReply(iface, packet, ntohs(header->tot_len), header->saddr, ICMP_TIME_EXCEEDED, ICMP_TIME_EXCEEDED_TTL);
        return;
    }

    iface->sendTo(iface, newPacket, ntohs(header->tot_len), macAddr, ETHERTYPE_IP);
    DEBUG_MSG("IP: packet forwarded\n");
}

void *IP::allocPacketFor(NetIface *iface, void *buf, int size, ipaddr destIP, int protocol, int *offset)
{
    macaddr macAddr = iface->macCache.value(destIP.addr);
    void *tmp = iface->allocPacketFor(iface, buf, size + sizeof(IPHeader), macAddr, ETHERTYPE_IP, offset);
    *offset += sizeof(IPHeader);

    return tmp;
}

void *IP::allocPacketFor(void *buf, int size, ipaddr destIP, int protocol, int *offset)
{
    macaddr macAddr;
    NetIface *iface;
    if (!route(destIP, &iface, &macAddr)){
        return 0;
    }

    void *tmp = iface->allocPacketFor(iface, buf, size + sizeof(IPHeader), macAddr, ETHERTYPE_IP, offset);
    *offset += sizeof(IPHeader);

    return tmp;
}

void IP::sendTo(NetIface *iface, void *buf, int size, ipaddr destIP, int protocol)
{
    macaddr macAddr = iface->macCache.value(destIP.addr);
    buildIPHeader(iface, ((uint8_t *) buf) + sizeof(EthernetIIHeader), destIP, protocol, sizeof(IPHeader) + size);
    iface->sendTo(iface, buf, sizeof(IPHeader) + size, macAddr, ETHERTYPE_IP);
}

void IP::sendTo(void *buf, int size, ipaddr destIP, int protocol)
{
    macaddr macAddr;
    NetIface *iface;
    if (!route(destIP, &iface, &macAddr)){
        return;
    }

    buildIPHeader(iface, ((uint8_t *) buf) + sizeof(EthernetIIHeader), destIP, protocol, sizeof(IPHeader) + size);
    iface->sendTo(iface, buf, sizeof(IPHeader) + size, macAddr, ETHERTYPE_IP);
}

