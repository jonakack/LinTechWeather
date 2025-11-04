#include "HTTPServerConnection.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

//-----------------Internal Functions-----------------

void HTTPServerConnection_TaskWork(void* _Context, uint64_t _MonTime);

//----------------------------------------------------

int HTTPServerConnection_Initiate(HTTPServerConnection* _Connection, int _FD)
{
	TCPClient_Initiate(&_Connection->tcpClient, _FD);

	// Initiera alla fält
	_Connection->method = NULL;
	_Connection->url = NULL;
	_Connection->context = NULL;
	_Connection->onRequest = NULL;
	_Connection->requestReceived = 0;
	
	_Connection->task = smw_createTask(_Connection, HTTPServerConnection_TaskWork);

	return 0;
}

int HTTPServerConnection_InitiatePtr(int _FD, HTTPServerConnection** _ConnectionPtr)
{
	if(_ConnectionPtr == NULL)
		return -1;

	HTTPServerConnection* _Connection = (HTTPServerConnection*)calloc(1, sizeof(HTTPServerConnection));
	if(_Connection == NULL)
		return -2;

	int result = HTTPServerConnection_Initiate(_Connection, _FD);
	if(result != 0)
	{
		free(_Connection);
		return result;
	}

	*(_ConnectionPtr) = _Connection;

	return 0;
}

void HTTPServerConnection_SetCallback(HTTPServerConnection* _Connection, void* _Context, HTTPServerConnection_OnRequest _OnRequest)
{
	_Connection->context = _Context;
	_Connection->onRequest = _OnRequest;
}

void HTTPServerConnection_TaskWork(void* _Context, uint64_t _MonTime)
{
	if (_Context == NULL)
		return;

	HTTPServerConnection* _Connection = (HTTPServerConnection*)_Context;

	// Om vi redan har processat requesten, gör inget mer
	if (_Connection->requestReceived)
		return;

	char buffer[4096];
	int bytesRead = TCPClient_Read(&_Connection->tcpClient, (uint8_t*)buffer, sizeof(buffer) - 1);
	
	// EAGAIN/EWOULDBLOCK = ingen data tillgänglig ännu
	if (bytesRead == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return;
		}
		return;
	}
	
	// Connection stängd. todo = dispose
	if (bytesRead == 0)
	{
		return;
	}
	
	// Null-terminera buffern
	buffer[bytesRead] = '\0';

	// Kolla om vi har hela HTTP meddelandet (slutar med \r\n\r\n)
	if (strstr(buffer, "\r\n\r\n") == NULL)
	{
		// Inkomplett request, vänta på mer data
		return;
	}
	
	// Parsa första raden: "GET /index.html HTTP/1.1"
	char method[16] = {0};
	char url[512] = {0};

	if (sscanf(buffer, "%15s %511s", method, url) == 2)
	{
		// Allokera och spara method
		size_t method_len = strlen(method);
		_Connection->method = (char*)malloc(method_len + 1);
		if (_Connection->method != NULL)
		{
			strcpy(_Connection->method, method);
		}
		
		// Allokera och spara url
		size_t url_len = strlen(url);
		_Connection->url = (char*)malloc(url_len + 1);
		if (_Connection->url != NULL)
		{
			strcpy(_Connection->url, url);
		}

		// Läs in alla headers (men ignorera dem för tillfället)
		char* header_start = strstr(buffer, "\r\n");
		if (header_start != NULL)
		{
			header_start += 2; // Hoppa över första \r\n
			
			// Loopa igenom alla headers tills vi hittar \r\n\r\n
			while (header_start != NULL && strncmp(header_start, "\r\n", 2) != 0)
			{
				char* header_end = strstr(header_start, "\r\n");
				if (header_end == NULL)
					break;
				
				// Här har vi en header-rad mellan header_start och header_end
				// Vi ignorerar den för tillfället
				
				header_start = header_end + 2;
			}
		}
		
		// Markera att vi har fått requesten
		_Connection->requestReceived = 1;
		
		// Om det är en HTTP GET, anropa callback
		if (strcmp(_Connection->method, "GET") == 0)
		{
			if (_Connection->onRequest != NULL)
			{
				_Connection->onRequest(_Connection->context); // ---- Anropa WeatherServerInstance_OnRequest ----
			}
		}
	}
}

void HTTPServerConnection_Dispose(HTTPServerConnection* _Connection)
{
	// Frigör allokerat minne
	if(_Connection->method != NULL)
	{
		free(_Connection->method);
		_Connection->method = NULL;
	}
	if(_Connection->url != NULL)
	{
		free(_Connection->url);
		_Connection->url = NULL;
	}
	
	TCPClient_Dispose(&_Connection->tcpClient);
	smw_destroyTask(_Connection->task);
}

void HTTPServerConnection_DisposePtr(HTTPServerConnection** _ConnectionPtr)
{
	if(_ConnectionPtr == NULL || *(_ConnectionPtr) == NULL)
		return;

	HTTPServerConnection_Dispose(*(_ConnectionPtr));
	free(*(_ConnectionPtr));
	*(_ConnectionPtr) = NULL;
}