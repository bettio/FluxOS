#ifndef _UDP_H_
#define _UDP_H_

#include <net/ip.h>

struct UDPHeader
{
    uint16_t sourceport;
    uint16_t destport;
    uint16_t length;
    uint16_t checksum;
} __attribute__ ((packed));

struct NetIface;

class UDP
{
    public:
        static void processUDPPacket(NetIface *iface, uint8_t *packet, int size);
        static void sendTo(NetIface *iface, ipaddr destIp, uint8_t *packet, int size);
};

#endif
