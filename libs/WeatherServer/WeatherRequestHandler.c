#include "WeatherRequestHandler.h"
#include "WeatherData.h"
#include "HTTPClient.h"
#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* WeatherRequestHandler_HandleGeoRequest(const char* _Url)
{
    if (_Url == NULL) 
    {
        return NULL;
    }

    // Parse geo request from URL
    GeoData* _GeoData = WeatherData_ParseGeoRequest(_Url);
    if (_GeoData == NULL) 
    {
        return NULL;
    }

    char* json_response = NULL;

    // Initialize cache configuration
    CacheConfig geo_cache = 
    {
        .base_path = "./libs/WeatherServer/cache/geodata",
        .ttl_seconds = 900  // 15 minutes
    };

    int cache_status = Cache_Check(_GeoData->city, &geo_cache);

    if (cache_status == UP_TO_DATE) 
    {
        json_response = Cache_Load(_GeoData->city, &geo_cache);
        if (json_response == NULL) 
        {
            printf("Failed to read cache, fetching new data\n");
            cache_status = DOES_NOT_EXIST; // Force fetch
        }
    }

    if (cache_status != UP_TO_DATE) 
    {
        // Fetch new data
        printf("Fetching new geo data for: %s\n", _GeoData->city);

        if (HTTPClient_GetGeoData(_GeoData) != 0) 
        {
            printf("Failed to fetch geo data\n");
            WeatherData_FreeGeoData(_GeoData);
            return NULL;
        }

        // Convert to JSON
        json_response = WeatherData_GeoToJson(_GeoData);

        if (json_response != NULL) 
        {
            if (Cache_Save(_GeoData->city, json_response, &geo_cache) != 0) {
                printf("Warning: Failed to save geo data to cache\n");
            }
        }
    }

    WeatherData_FreeGeoData(_GeoData);
    return json_response;
}

char* WeatherRequestHandler_HandleWeatherRequest(const char* _Url)
{
    if (_Url == NULL) 
    {
        return NULL;
    }

    WeatherData* _WeatherData = WeatherData_ParseWeatherRequest(_Url);
    if (_WeatherData == NULL) 
    {
        return NULL;
    }

    char* json_response = NULL;

    // Initialize cache configuration
    CacheConfig weather_cache = 
    {
        .base_path = "./libs/WeatherServer/cache/weatherdata",
        .ttl_seconds = 900  // 15 minutes
    };

    char cache_key[256];
    snprintf(cache_key, sizeof(cache_key), "%s_%s",
             _WeatherData->latitude, _WeatherData->longitude);

    int cache_status = Cache_Check(cache_key, &weather_cache);

    if (cache_status == UP_TO_DATE) 
    {
        json_response = Cache_Load(cache_key, &weather_cache);

        if (json_response == NULL) 
        {
            printf("Failed to read cache, fetching new data\n");
            cache_status = DOES_NOT_EXIST; // Force fetch
        }
    }

    if (cache_status != UP_TO_DATE) 
    {
        // Fetch new data
        printf("Fetching new weather data for: %s, %s\n",
               _WeatherData->latitude, _WeatherData->longitude);

        if (HTTPClient_GetWeatherData(_WeatherData) != 0) 
        {
            printf("Failed to fetch weather data\n");
            WeatherData_FreeWeatherData(_WeatherData);
            return NULL;
        }

        // Convert to JSON
        json_response = WeatherData_WeatherToJson(_WeatherData);

        // Save to cache using generic API
        if (json_response != NULL) 
        {
            if (Cache_Save(cache_key, json_response, &weather_cache) != 0) {
                printf("Warning: Failed to save weather data to cache\n");
            }
        }
    }

    // Free weather data
    WeatherData_FreeWeatherData(_WeatherData);

    return json_response;
}

int WeatherRequestHandler_ParseHTTPRequest(const char* _RequestLine, HTTPRequest* _ParsedRequest)
{
    if (!_RequestLine || !_ParsedRequest) 
    {
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
    if (token) 
    {
        // Split path and query string at '?'
        char* query_start = strchr(token, '?');
        if (query_start) 
        {
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
    if (token) 
    {
        strncpy(_ParsedRequest->version, token, sizeof(_ParsedRequest->version) - 1);
    }

    return 0; // Success
}