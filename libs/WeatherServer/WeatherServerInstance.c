#include "WeatherServerInstance.h"

//-----------------Internal Functions-----------------

int WeatherServerInstance_OnRequest(void* _Context);

//----------------------------------------------------

int WeatherServerInstance_Initiate(WeatherServerInstance* _Instance, HTTPServerConnection* _Connection)
{
	_Instance->connection = _Connection;
	_Instance->completed = 0;

	HTTPServerConnection_SetCallback(_Instance->connection, _Instance, WeatherServerInstance_OnRequest);

	return 0;
}

int WeatherServerInstance_InitiatePtr(HTTPServerConnection* _Connection, WeatherServerInstance** _InstancePtr)
{
	if(_InstancePtr == NULL)
		return -1;

	WeatherServerInstance* _Instance = (WeatherServerInstance*)malloc(sizeof(WeatherServerInstance));
	if(_Instance == NULL)
		return -2;

	int result = WeatherServerInstance_Initiate(_Instance, _Connection);
	if(result != 0)
	{
		free(_Instance);
		return result;
	}

	*(_InstancePtr) = _Instance;

	return 0;
}

int WeatherServerInstance_OnRequest(void* _Context)
{
    WeatherServerInstance* _Instance = (WeatherServerInstance*)_Context;
    HTTPServerConnection* _Connection = _Instance->connection;
    WeatherRequest_HandleRequest(_Connection);
    return 0;
}

void WeatherServerInstance_Work(WeatherServerInstance* _Instance, uint64_t _MonTime)
{
	// Check if the HTTP connection has finished processing the request
	if (_Instance->connection && _Instance->connection->requestReceived && !_Instance->completed) {
		// Mark this instance as completed and ready for cleanup
		_Instance->completed = 1;
		// printf("WeatherServerInstance: Instance completed, marked for cleanup\n");
	}
}

void WeatherServerInstance_Dispose(WeatherServerInstance* _Instance)
{

}

void WeatherServerInstance_DisposePtr(WeatherServerInstance** _InstancePtr)
{
	if(_InstancePtr == NULL || *(_InstancePtr) == NULL)
		return;

	WeatherServerInstance_Dispose(*(_InstancePtr));
	free(*(_InstancePtr));
	*(_InstancePtr) = NULL;
}
