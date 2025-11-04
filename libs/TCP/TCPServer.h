
#ifndef __TCPServer_h_
#define __TCPServer_h_

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "smw.h"

#define MAX_CLIENTS 10

typedef int (*TCPServer_OnAccept)(int client_fd, void* context);

typedef struct
{
	int listen_fd;

	TCPServer_OnAccept onAccept;
	void* context;

	smw_task* task;

} TCPServer;


int TCPServer_Initiate(TCPServer* _Server, const char* _Port, TCPServer_OnAccept _OnAccept, void* _Context);
int TCPServer_InitiatePtr(const char* _Port, TCPServer_OnAccept _OnAccept, void* _Context, TCPServer** _ServerPtr);


void TCPServer_Dispose(TCPServer* _Server);
void TCPServer_DisposePtr(TCPServer** _ServerPtr);

static inline int TCPServer_Nonblocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return -1;

	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

#endif //__TCPServer_h_
