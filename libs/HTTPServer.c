#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "HTTPServer.h"

int HTTPServer_OnAccept(int _Socketfd, void* context)
{
    HTTPServer* _Server = (HTTPServer*)context;
}

int HTTPServer_Init(HTTPServer *_Server)
{
    TCPServer_Initiate(&_Server->tcpserver, "8080", &HTTPServer_OnAccept);
}