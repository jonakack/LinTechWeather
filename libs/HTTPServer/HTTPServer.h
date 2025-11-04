
#ifndef __HTTPServer_h_
#define __HTTPServer_h_

#include "smw.h"
#include "HTTPServerConnection.h"
#include "../TCP/TCPServer.h"

typedef int (*HTTPServer_OnConnection)(void* _Context, HTTPServerConnection* _Connection);

typedef struct
{
	HTTPServer_OnConnection onConnection;

	TCPServer tcpServer;
	smw_task* task;

} HTTPServer;


int HTTPServer_Initiate(HTTPServer* _Server, HTTPServer_OnConnection _OnConnection);
int HTTPServer_InitiatePtr(HTTPServer_OnConnection _OnConnection, HTTPServer** _ServerPtr);


void HTTPServer_Dispose(HTTPServer* _Server);
void HTTPServer_DisposePtr(HTTPServer** _ServerPtr);


#endif //__HTTPServer_h_
