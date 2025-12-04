#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "HTTPServerConnection.h"

//-----------------Internal Functions-----------------

void HTTPServerConnection_TaskWork(void* _Context, uint64_t _MonTime);

//----------------------------------------------------

int HTTPServerConnection_Initiate(HTTPServerConnection* _Connection, int _FD)
{
	TCPClient_Initiate(&_Connection->tcpClient, _FD);

	// Initialize all fields
	_Connection->method = NULL;
	_Connection->url = NULL;
	_Connection->requestString = NULL;
	_Connection->raw_request = NULL;
	_Connection->context = NULL;
	_Connection->onRequest = NULL;
	_Connection->requestReceived = 0;

	// Initialize state machine fields
	_Connection->state = HTTPServerConnectionState_ReceiveRequest;
	_Connection->receiveBufferCapacity = 4096;
	_Connection->receiveBuffer = malloc(_Connection->receiveBufferCapacity);
	_Connection->receiveBufferSize = 0;
	_Connection->responseBuffer = NULL;
	_Connection->responseLength = 0;
	_Connection->bytesSent = 0;
	_Connection->callbackInvoked = 0;

	if(_Connection->receiveBuffer == NULL)
		return -1;

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

void HTTPServerConnection_SetResponse(HTTPServerConnection* _Connection, const char* _Response, size_t _Length)
{
	if(_Connection->responseBuffer != NULL)
	{
		free(_Connection->responseBuffer);
		_Connection->responseBuffer = NULL;
	}

	_Connection->responseBuffer = malloc(_Length);
	if(_Connection->responseBuffer != NULL)
	{
		memcpy(_Connection->responseBuffer, _Response, _Length);
		_Connection->responseLength = _Length;
	}
	else
	{
		_Connection->responseLength = 0;
	}
}

void HTTPServerConnection_TaskWork(void* _Context, uint64_t _MonTime)
{
	if (_Context == NULL)
		return;

	HTTPServerConnection* _Connection = (HTTPServerConnection*)_Context;

	switch (_Connection->state)
	{
		case HTTPServerConnectionState_ReceiveRequest:
		{
			// Check if we need to expand buffer
			size_t availableSpace = _Connection->receiveBufferCapacity - _Connection->receiveBufferSize;

			if (availableSpace < 512)
			{
				// Need to expand buffer
				size_t newCapacity = _Connection->receiveBufferCapacity * 2;
				char* newBuffer = realloc(_Connection->receiveBuffer, newCapacity);
				if (newBuffer == NULL)
				{
					printf("HTTPServerConnection: Failed to expand receive buffer\n");
					_Connection->state = HTTPServerConnectionState_Close;
					break;
				}
				_Connection->receiveBuffer = newBuffer;
				_Connection->receiveBufferCapacity = newCapacity;
				availableSpace = newCapacity - _Connection->receiveBufferSize;
			}

			// Read data incrementally
			int bytesRead = TCPClient_Read(&_Connection->tcpClient,
											(uint8_t*)(_Connection->receiveBuffer + _Connection->receiveBufferSize),
											availableSpace);

			if (bytesRead > 0)
			{
				_Connection->receiveBufferSize += bytesRead;
				_Connection->receiveBuffer[_Connection->receiveBufferSize] = '\0';

				// Check if we have complete request (ends with \r\n\r\n)
				if (strstr(_Connection->receiveBuffer, "\r\n\r\n") != NULL)
				{
					_Connection->state = HTTPServerConnectionState_ProcessRequest;
				}
			}
			else if (bytesRead == 0)
			{
				// Connection closed by client
				printf("HTTPServerConnection: Client closed connection during receive\n");
				_Connection->state = HTTPServerConnectionState_Close;
			}
			else if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				// Real error
				printf("HTTPServerConnection: Read error during receive\n");
				_Connection->state = HTTPServerConnectionState_Close;
			}
			// else: EAGAIN/EWOULDBLOCK - just wait for more data
			break;
		}

		case HTTPServerConnectionState_ProcessRequest:
		{
			// Store the full raw request
			size_t bufferLen = strlen(_Connection->receiveBuffer);
			_Connection->raw_request = (char*)malloc(bufferLen + 1);
			if (_Connection->raw_request != NULL)
			{
				strcpy(_Connection->raw_request, _Connection->receiveBuffer);
			}

			// Parse first line: "GET /index.html HTTP/1.1"
			char method[16] = {0};
			char url[512] = {0};

			if (sscanf(_Connection->receiveBuffer, "%15s %511s", method, url) == 2)
			{
				// Allocate and save method
				size_t methodLen = strlen(method);
				_Connection->method = (char*)malloc(methodLen + 1);
				if (_Connection->method != NULL)
				{
					strncpy(_Connection->method, method, methodLen);
					_Connection->method[methodLen] = '\0';
				}

				// Allocate and save url
				size_t urlLen = strlen(url);
				_Connection->url = (char*)malloc(urlLen + 1);
				if (_Connection->url != NULL)
				{
					strncpy(_Connection->url, url, urlLen);
					_Connection->url[urlLen] = '\0';
				}

				// Mark that we have received the request
				_Connection->requestReceived = 1;

				// Move to callback state
				_Connection->state = HTTPServerConnectionState_WaitingCallback;
			}
			else
			{
				// Parse error
				printf("HTTPServerConnection: Failed to parse request\n");
				_Connection->state = HTTPServerConnectionState_Close;
			}
			break;
		}

		case HTTPServerConnectionState_WaitingCallback:
		{
			// Call user callback ONCE if it's a GET request
			if (!_Connection->callbackInvoked && strcmp(_Connection->method, "GET") == 0 && _Connection->onRequest != NULL)
			{
				_Connection->onRequest(_Connection->context);
				_Connection->callbackInvoked = 1;
			}

			// Check if response has been set (either immediately or by async callback later)
			if (_Connection->responseBuffer != NULL && _Connection->responseLength > 0)
			{
				_Connection->state = HTTPServerConnectionState_SendResponse;
				_Connection->bytesSent = 0;
			}
			// Else: Stay in WaitingCallback state waiting for async response
			break;
		}

		case HTTPServerConnectionState_SendResponse:
		{
			int bytesToSend= _Connection->responseLength - _Connection->bytesSent;
			int sent = TCPClient_Write(&_Connection->tcpClient, (uint8_t*)(_Connection->responseBuffer + _Connection->bytesSent), bytesToSend);

			if (sent > 0)
			{
				_Connection->bytesSent += sent;

				if (_Connection->bytesSent >= _Connection->responseLength)
				{
					// All sent, now we can close
					printf("HTTPServerConnection: Response sent completely (%zu bytes)\n", _Connection->responseLength);
					_Connection->state = HTTPServerConnectionState_Close;
				}
			}
			else if (sent == 0)
			{
				// Connection closed
				printf("HTTPServerConnection: Connection closed during send\n");
				_Connection->state = HTTPServerConnectionState_Close;
			}
			else if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				// Real error
				printf("HTTPServerConnection: Write error during send\n");
				_Connection->state = HTTPServerConnectionState_Close;
			}
			// else: EAGAIN/EWOULDBLOCK - try again next iteration
			break;
		}

		case HTTPServerConnectionState_Close:
		{
			printf("HTTPServerConnection: Closing connection\n");
			HTTPServerConnection_Dispose(_Connection);
			// Task will be destroyed in Dispose, so we must return
			return;
		}
	}
}


void HTTPServerConnection_Dispose(HTTPServerConnection* _Connection)
{
	// Free allocated memory
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
	if(_Connection->requestString != NULL)
	{
		free(_Connection->requestString);
		_Connection->requestString = NULL;
	}
	if(_Connection->raw_request != NULL)
	{
		free(_Connection->raw_request);
		_Connection->raw_request = NULL;
	}

	// Free state machine buffers
	if(_Connection->receiveBuffer != NULL)
	{
		free(_Connection->receiveBuffer);
		_Connection->receiveBuffer = NULL;
	}
	if(_Connection->responseBuffer != NULL)
	{
		free(_Connection->responseBuffer);
		_Connection->responseBuffer = NULL;
	}

	TCPClient_Dispose(&_Connection->tcpClient);

	if(_Connection->task != NULL)
	{
		smw_destroyTask(_Connection->task);
		_Connection->task = NULL;
	}
}

void HTTPServerConnection_DisposePtr(HTTPServerConnection** _ConnectionPtr)
{
	if(_ConnectionPtr == NULL || *(_ConnectionPtr) == NULL)
		return;

	HTTPServerConnection_Dispose(*(_ConnectionPtr));
	free(*(_ConnectionPtr));
	*(_ConnectionPtr) = NULL;
}

void HTTPServerConnection_EchoRequest(HTTPServerConnection* _Connection)
{
    char response[8192];  // Increased size for full request
    
    // Echo back what we parsed
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Method: %s\n"
        "URL: %s\n"
        "Request Received: %s\n"
        "\n--- Full Request with Headers ---\n"
        "%s"
        "--- End Request ---\n",
        _Connection->method ? _Connection->method : "NULL",
        _Connection->url ? _Connection->url : "NULL", 
        _Connection->requestReceived ? "YES" : "NO",
        _Connection->raw_request ? _Connection->raw_request : "No raw request stored"
    );
    
    // Send response back through TCPClient
    TCPClient_Write(&_Connection->tcpClient, (uint8_t*)response, strlen(response));
}