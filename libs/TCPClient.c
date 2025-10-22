#include "TCPClient.h"

int TCPClient_Initiate(TCPClient* _Client)
{
    _Client->fd =-1;
    return 0;
}

int TCPClient_Connect(TCPClient* _Client, const char* host, const char* port)
{
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &hints, &res) != 0)
        return -1;

    
    int fd = -1;
    for (struct addrinfo *resPtr = res; resPtr; resPtr = resPtr->ai_next)
	{
        fd = socket(resPtr->ai_family, resPtr->ai_socktype, resPtr->ai_protocol);

        if (fd < 0)
			continue;

        if (connect(fd, resPtr->ai_addr, resPtr->ai_addrlen) == 0)
			break;

        close(fd);
        fd = -1;
    }

    freeaddrinfo(res);
    if (fd < 0)
		return -1;

    _Client->fd = fd;
    return 0;
}

int TCPClient_Write(TCPClient* _Client, const uint8_t* buf, int len)
{
    return send(_Client->fd, buf, len, MSG_NOSIGNAL);
}

int TCPClient_Read(TCPClient* _Client, uint8_t* buf, int len)
{
    return recv(_Client->fd, buf, len, MSG_DONTWAIT);
}

void TCPClient_Disconnect(TCPClient* _Client)
{
    if (_Client->fd >= 0)
		close(_Client->fd);

    _Client->fd = -1;
}

void TCPClient_Dispose(TCPClient* _Client)
{
	TCPClient_Disconnect(_Client);
}