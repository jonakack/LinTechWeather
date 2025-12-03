#ifndef __WeatherData_h_
#define __WeatherData_h_

#include "HTTPServerConnection.h"

typedef struct {

    char* city;
    char* latitude;
    char* longitude;
    char* response;
    
    int request_complete;

} WeatherData;

// Parse the URL and return struct data
WeatherData* WeatherData_ParseRequest(const char* _Url);

// Convert data to JSON string
char *WeatherData_HttpResponseToJson(const char *response);

// Free the data structure
void WeatherData_Dispose(WeatherData* _Data);

#endif // __WeatherData_h_