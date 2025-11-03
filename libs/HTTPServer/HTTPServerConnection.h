
#ifndef __HTTPServerConnection_h_
#define __HTTPServerConnection_h_

#include "smw.h"
#include "../TCPClient.h"

typedef int (*HTTPServerConnection_OnRequest)(void* _Context);

typedef struct
{
	TCPClient tcpClient;

	void* context;
	HTTPServerConnection_OnRequest onRequest;

	char* method;
	char* url;

	smw_task* task;

} HTTPServerConnection;


int HTTPServerConnection_Initiate(HTTPServerConnection* _Connection, int _FD);
int HTTPServerConnection_InitiatePtr(int _FD, HTTPServerConnection** _ConnectionPtr);

void HTTPServerConnection_SetCallback(HTTPServerConnection* _Connection, void* _Context, HTTPServerConnection_OnRequest _OnRequest);

void HTTPServerConnection_Dispose(HTTPServerConnection* _Connection);
void HTTPServerConnection_DisposePtr(HTTPServerConnection** _ConnectionPtr);

#endif //__HTTPServerConnection_h_
