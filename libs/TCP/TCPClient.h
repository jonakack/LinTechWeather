#ifndef __TCPClient_h_
#define __TCPClient_h_

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct TCPClient
{ 
    int fd;

} TCPClient;

int TCPClient_Initiate(TCPClient* _Client, int _FD);

int TCPClient_Connect(TCPClient* _Client, const char* _Host, const char* _Port);

int TCPClient_Write(TCPClient* _Client, const uint8_t* _Buffer, int _Length);
int TCPClient_Read(TCPClient* _Client, uint8_t* _Buffer, int _Length);

void TCPClient_Disconnect(TCPClient* _Client);

void TCPClient_Dispose(TCPClient* _Client);

#endif // __TCPClient_h_