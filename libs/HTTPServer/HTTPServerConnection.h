#ifndef __HTTPServerConnection_h_
#define __HTTPServerConnection_h_

#include "smw.h"
#include "TCPClient.h"
#include <stddef.h>

typedef int (*HTTPServerConnection_OnRequest)(void* _Context);

typedef enum
{
	HTTPServerConnectionState_ReceiveRequest,
	HTTPServerConnectionState_ProcessRequest,
	HTTPServerConnectionState_WaitingCallback,
	HTTPServerConnectionState_SendResponse,
	HTTPServerConnectionState_Close
} HTTPServerConnectionState;

typedef struct
{
	TCPClient tcpClient;

	void* context;
	HTTPServerConnection_OnRequest onRequest;
	char* method;
	char* url;
	char* requestString;
	char* raw_request;
	int requestReceived;

	// State machine fields
	HTTPServerConnectionState state;
	char* receiveBuffer;
	size_t receiveBufferSize;
	size_t receiveBufferCapacity;
	char* responseBuffer;
	size_t responseLength;
	size_t bytesSent;
	int callbackInvoked; 

	smw_task* task;

} HTTPServerConnection;


int HTTPServerConnection_Initiate(HTTPServerConnection* _Connection, int _FD);
int HTTPServerConnection_InitiatePtr(int _FD, HTTPServerConnection** _ConnectionPtr);

void HTTPServerConnection_SetCallback(HTTPServerConnection* _Connection, void* _Context, HTTPServerConnection_OnRequest _OnRequest);
void HTTPServerConnection_SetResponse(HTTPServerConnection* _Connection, const char* _Response, size_t _Length);
void HTTPServerConnection_TaskWork(void* _Context, uint64_t _MonTime);
void HTTPServerConnection_Dispose(HTTPServerConnection* _Connection);
void HTTPServerConnection_DisposePtr(HTTPServerConnection** _ConnectionPtr);

void HTTPServerConnection_EchoRequest(HTTPServerConnection* _Connection);

#endif //__HTTPServerConnection_h_
