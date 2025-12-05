#ifndef HTTPClient_h
#define HTTPClient_h

#include <stdint.h>
#include "TaskScheduler.h"
#include "WeatherData.h"
#include "TCPClient.h"

typedef enum
{
	HTTPClient_State_Init,
	HTTPClient_State_Connect,
	HTTPClient_State_Transmit,
	HTTPClient_State_Receive,
	HTTPClient_State_Close
} HTTPClient_State;

typedef struct HTTPClient HTTPClient;

struct HTTPClient
{
	void (*callback)(HTTPClient* _Client, const char* _Event);
	uint8_t* buffer;
	uint8_t* bufferPtr;
	Task* task;
	TaskScheduler* scheduler;  // Reference to task scheduler

	HTTPClient_State state;
	char host[256];
	char port[8];
	WeatherData* weather_data;
	TCPClient tcpClient;
	int bytes_sent;
	int total_received;
};

int HTTPClient_Initiate(HTTPClient* _Client, TaskScheduler* _Scheduler);
int HTTPClient_GET(HTTPClient* _Client, const char* _URL, void (*callback)(HTTPClient* _Client, const char* _Event));
void HTTPClient_Dispose(HTTPClient* _Client);

// High-level functions for weather data
int HTTPClient_GetGeoData(WeatherData* _Data, TaskScheduler* _Scheduler);
int HTTPClient_GetWeatherData(WeatherData* _Data, TaskScheduler* _Scheduler);

// Callbacks
void HTTPClient_GeoCallback(HTTPClient* _Client, const char* _Event);
void HTTPClient_WeatherCallback(HTTPClient* _Client, const char* _Event);

#endif //HTTPClient_h