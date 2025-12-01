#include "WeatherRequestHandler.h"
#include "WeatherData.h"
#include <stdlib.h>


char *WeatherRequestHandler_HandleGeoRequest(const char *_Url)
{
    if (_Url == NULL)
    {
        return NULL;
    }

    // Parse geo request from URL
    GeoData *geo_data = WeatherData_ParseGeoRequest(_Url);
    if (geo_data == NULL)
    {
        return NULL;
    }

    // Convert to JSON
    char *json_response = WeatherData_GeoToJson(geo_data);

    // Free geo data
    WeatherData_FreeGeoData(geo_data);

    return json_response;
}

char *WeatherRequestHandler_HandleWeatherRequest(const char *_Url)
{
    if (_Url == NULL)
    {
        return NULL;
    }

    // Parse weather request from URL
    WeatherData *weather_data = WeatherData_ParseWeatherRequest(_Url);
    if (weather_data == NULL)
    {
        return NULL;
    }

    // Convert to JSON
    char *json_response = WeatherData_WeatherToJson(weather_data);

    // Free weather data
    WeatherData_FreeWeatherData(weather_data);

    return json_response;
}

int WeatherRequestHandler_ParseHTTPRequest(const char* _RequestLine, HTTPRequest* _ParsedRequest)
{
    if (!_RequestLine || !_ParsedRequest) {
        return -1;
    }
    
    // Initialize the struct
    memset(_ParsedRequest, 0, sizeof(HTTPRequest));
    
    // Copy request line for parsing (strtok modifies the string)
    char request_copy[1024];
    strncpy(request_copy, _RequestLine, sizeof(request_copy) - 1);
    request_copy[sizeof(request_copy) - 1] = '\0';
    
    // Parse: "GET /api/v1/weather?lat=59.3293&lon=18.0686 HTTP/1.1"
    char* token;
    
    // Get method (GET, POST, etc.)
    token = strtok(request_copy, " ");
    if (token) {
        strncpy(_ParsedRequest->method, token, sizeof(_ParsedRequest->method) - 1);
    }
    
    // Get URL (path + query string)
    token = strtok(NULL, " ");
    if (token) {
        // Split path and query string at '?'
        char* query_start = strchr(token, '?');
        if (query_start) {
            // Copy path (everything before '?')
            size_t path_len = query_start - token;
            strncpy(_ParsedRequest->path, token, 
                   path_len < sizeof(_ParsedRequest->path) - 1 ? path_len : sizeof(_ParsedRequest->path) - 1);
            
            // Copy query string (everything after '?')
            strncpy(_ParsedRequest->query_string, query_start + 1, sizeof(_ParsedRequest->query_string) - 1);
        } else {
            // No query string, just path
            strncpy(_ParsedRequest->path, token, sizeof(_ParsedRequest->path) - 1);
        }
    }
    
    // Get HTTP version
    token = strtok(NULL, " ");
    if (token) {
        strncpy(_ParsedRequest->version, token, sizeof(_ParsedRequest->version) - 1);
    }
    
    return 0; // Success
}