#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include <QList>

struct sockaddr;

struct TCPSocket
{
    int lowerLevelProtocol;
    void *lowerLevelSocket;
    sockaddr *localAddr;
    sockaddr *remoteAddr;
    QList<void *> *datagrams;
    QList<TCPSocket *> *queuedConnections;

    uint32_t seqnumber;
};

#endif

