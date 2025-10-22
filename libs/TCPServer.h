#ifndef __TCPSERVER_H
#define __TCPSERVER_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "TCPClient.h"

#define MAX_CLIENTS 10

typedef struct
{
    int listen_fd;
    TCPClient clients[MAX_CLIENTS];

} TCPServer;

int TCPServer_Initiate(TCPServer* _Server, const char * _Port);

// int TCPServer_Listen();

int TCPServer_Accept(TCPServer* _Server);

void TCPServer_Work(TCPServer* _Server);

void TCPServer_Dispose(TCPServer* _Server);

static inline int TCPServer_Nonblocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return -1;

	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

#endif // __TCPSERVER_H