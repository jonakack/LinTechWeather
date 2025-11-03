#include "TCPServer.h"
#include <stdlib.h>

//-----------------Internal Functions-----------------

void TCPServer_TaskWork(void* _Context, uint64_t _MonTime);

//----------------------------------------------------

int TCPServer_Initiate(TCPServer* _Server, const char* _Port, TCPServer_OnAccept _OnAccept, void* _Context)
{
	_Server->onAccept = _OnAccept;
	_Server->context = _Context;

	struct addrinfo hints = {0}, *res = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, _Port, &hints, &res) != 0)
		return -1;

	int fd = -1;
	for (struct addrinfo *rp = res; rp; rp = rp->ai_next)
	{
		fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (fd < 0)
			continue;

		int yes = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		close(fd);
		fd = -1;
	}

	freeaddrinfo(res);
	if (fd < 0)
		return -1;

	if (listen(fd, MAX_CLIENTS) < 0)
	{
		close(fd);
		return -1;
	}

	TCPServer_Nonblocking(fd);
	
	_Server->listen_fd = fd;

	_Server->task = smw_createTask(_Server, TCPServer_TaskWork);

	return 0;
}

int TCPServer_InitiatePtr(const char* _Port, TCPServer_OnAccept _OnAccept, void* _Context, TCPServer** _ServerPtr)
{
	if(_ServerPtr == NULL)
		return -1;

	TCPServer* _Server = (TCPServer*)malloc(sizeof(TCPServer));
	if(_Server == NULL)
		return -2;

	int result = TCPServer_Initiate(_Server, _Port, _OnAccept, _Context);
	if(result != 0)
	{
		free(_Server);
		return result;
	}

	*(_ServerPtr) = _Server;

	return 0;
}

int TCPServer_Accept(TCPServer* _Server)
{
	int socket_fd = accept(_Server->listen_fd, NULL, NULL);
	if (socket_fd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return 0; // ingen ny klient

		perror("accept");
		return -1;
	}

	TCPServer_Nonblocking(socket_fd);

	int result = _Server->onAccept(socket_fd, _Server->context);
	if(result != 0)
		close(socket_fd);

	return 0;
}

void TCPServer_TaskWork(void* _Context, uint64_t _MonTime)
{
	TCPServer* _Server = (TCPServer*)_Context;
	
	TCPServer_Accept(_Server);
}

void TCPServer_Dispose(TCPServer* _Server)
{
	smw_destroyTask(_Server->task);
}

void TCPServer_DisposePtr(TCPServer** _ServerPtr)
{
	if(_ServerPtr == NULL || *(_ServerPtr) == NULL)
		return;

	TCPServer_Dispose(*(_ServerPtr));
	free(*(_ServerPtr));
	*(_ServerPtr) = NULL;
}
