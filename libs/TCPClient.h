#ifndef __TCPCLIENT_H
#define __TCPCLIENT_H

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

int TCPClient_Initiate(TCPClient* _Client);

int TCPClient_Connect(TCPClient* _Client, const char* host, const char* port);

int TCPClient_Write(TCPClient* _Client, const uint8_t* buf, int len);
int TCPClient_Read(TCPClient* _Client, uint8_t* buf, int len);

void TCPClient_Disconnect(TCPClient* _Client);

void TCPClient_Dispose(TCPClient* _Client);

#endif // __TCPCLIENT_H