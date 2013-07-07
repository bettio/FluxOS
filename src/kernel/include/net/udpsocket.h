#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_

struct sockaddr;

struct IPSocket
{

};

struct UDPSocket
{
    int lowerLevelProtocol;
    void *lowerLevelSocket;
    sockaddr *localAddr;
    sockaddr *remoteAddr;
};

#endif

