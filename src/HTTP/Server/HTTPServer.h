#ifndef __HTTPServer_h_
#define __HTTPServer_h_

#include "TaskScheduler.h"
#include "HTTPServerConnection.h"
#include "TCPServer.h"

// Default port if not defined at compile time
#ifndef HTTP_SERVER_PORT
#define HTTP_SERVER_PORT "8080"
#endif

typedef int (*HTTPServer_OnConnection)(void* _Context, HTTPServerConnection* _Connection);

typedef struct
{
	HTTPServer_OnConnection onConnection;

	TCPServer tcpServer;
	Task* task;
	TaskScheduler* scheduler;  // Reference to task scheduler

} HTTPServer;


int HTTPServer_Initiate(HTTPServer* _Server, TaskScheduler* _Scheduler, HTTPServer_OnConnection _OnConnection);
int HTTPServer_InitiatePtr(HTTPServer_OnConnection _OnConnection, TaskScheduler* _Scheduler, HTTPServer** _ServerPtr);


void HTTPServer_Dispose(HTTPServer* _Server);
void HTTPServer_DisposePtr(HTTPServer** _ServerPtr);


#endif //__HTTPServer_h_
