#include "WeatherServerInstance.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

//-----------------Internal Functions-----------------

int WeatherServerInstance_OnRequest(void* _Context);

//----------------------------------------------------

int WeatherServerInstance_Initiate(WeatherServerInstance* _Instance, HTTPServerConnection* _Connection)
{
	_Instance->connection = _Connection;

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

    char* json_response = NULL;
    const char* url = _Connection->url;

    // Try parsing as geo request first
    GeoData* geo_data = WeatherData_ParseGeoRequest(url);
	WeatherData* weather_data = WeatherData_ParseWeatherRequest(url);
    if (geo_data) {
        json_response = WeatherData_GeoToJson(geo_data);
        WeatherData_FreeGeoData(geo_data);
    // If not a geo, try as weather.
    } else {
		json_response = WeatherData_WeatherToJson(weather_data);
		WeatherData_FreeWeatherData(weather_data);
    }

    if (json_response == NULL) {
        // Handle error - send 400 Bad Request with correct Content-Length
        const char* body = "{\"error\":\"Invalid request\"}";
        size_t body_len = strlen(body);

        char header[256];
        int header_len = snprintf(header, sizeof(header),
                                  "HTTP/1.1 400 Bad Request\r\n"
                                  "Content-Type: application/json\r\n"
                                  "Content-Length: %zu\r\n"
                                  "\r\n",
                                  body_len);
        if (header_len > 0) {
            TCPClient_Write(&_Connection->tcpClient, (uint8_t*)header, header_len);
        }
        TCPClient_Write(&_Connection->tcpClient, (uint8_t*)body, (int)body_len);
        return 0;
    }

    // Create HTTP response
    char response_header[256];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %zu\r\n"
             "\r\n",
             strlen(json_response));

    // Send response
    TCPClient_Write(&_Connection->tcpClient, (uint8_t*)response_header, strlen(response_header));
    TCPClient_Write(&_Connection->tcpClient, (uint8_t*)json_response, strlen(json_response));

    // Clean up
    free(json_response);

    return 0;
}

void WeatherServerInstance_Work(WeatherServerInstance* _Server, uint64_t _MonTime)
{
	
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
