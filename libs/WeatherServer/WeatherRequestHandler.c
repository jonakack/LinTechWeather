#include "WeatherRequestHandler.h"
#include "WeatherData.h"
#include "HTTPClient.h"
#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* WeatherRequestHandler_HandleGeoRequest(const char* _Url)
{
    if (_Url == NULL) {
        return NULL;
    }
    
    // Parse geo request from URL
    GeoData* geo_data = WeatherData_ParseGeoRequest(_Url);
    if (geo_data == NULL) {
        return NULL;
    }
    
    // Create AllData wrapper for cache functions
    AllData allData = {0};
    allData.GeoData = geo_data;
    
    char* json_response = NULL;
    
    // Check if file exists and is up to date
    int cache_status = Cache_CheckExisting(&allData, IS_GEODATA);
    
    if (cache_status == UP_TO_DATE) {
        // Read from cache file
        char filename[256];
        snprintf(filename, sizeof(filename), "../../cache/geodata/%s.json", geo_data->city);
        
        printf("Reading cached geo data from: %s\n", filename);
        
        FILE* fptr = fopen(filename, "r");
        if (fptr != NULL) {
            // Get file size
            fseek(fptr, 0, SEEK_END);
            long file_size = ftell(fptr);
            fseek(fptr, 0, SEEK_SET);
            
            // Allocate buffer and read
            json_response = malloc(file_size + 1);
            if (json_response != NULL) {
                fread(json_response, 1, file_size, fptr);
                json_response[file_size] = '\0';
            }
            fclose(fptr);
        }
        
        if (json_response == NULL) {
            printf("Failed to read cache, fetching new data\n");
            cache_status = DOES_NOT_EXIST; // Force fetch
        }
    }
    
    if (cache_status != UP_TO_DATE) {
        // Fetch new data
        printf("Fetching new geo data for: %s\n", geo_data->city);
        
        if (HTTPClient_GetGeoData(geo_data) != 0) {
            printf("Failed to fetch geo data\n");
            WeatherData_FreeGeoData(geo_data);
            return NULL;
        }
        
        // Convert to JSON
        json_response = WeatherData_GeoToJson(geo_data);
        
        if (json_response != NULL) {
            // Save to cache
            if (Cache_SaveData(&allData, "geo") != 0) {
                printf("Warning: Failed to save to cache\n");
            }
        }
    }
    
    // Free geo data
    WeatherData_FreeGeoData(geo_data);
    
    return json_response;
}

char* WeatherRequestHandler_HandleWeatherRequest(const char* _Url)
{
    if (_Url == NULL) {
        return NULL;
    }
    
    // Parse weather request from URL
    WeatherData* weather_data = WeatherData_ParseWeatherRequest(_Url);
    if (weather_data == NULL) {
        return NULL;
    }
    
    // Create AllData wrapper for cache functions
    AllData allData = {0};
    allData.WeatherData = weather_data;
    
    char* json_response = NULL;
    
    // Check if file exists and is up to date
    int cache_status = Cache_CheckExisting(&allData, IS_WEATHERDATA);
    
    if (cache_status == UP_TO_DATE) {
        // Read from cache file
        char filename[256];
        snprintf(filename, sizeof(filename), 
                 "../../cache/weatherdata/%s_%s.json",
                 weather_data->latitude, weather_data->longitude);
        
        printf("Reading cached weather data from: %s\n", filename);
        
        FILE* fptr = fopen(filename, "r");
        if (fptr != NULL) {
            // Get file size
            fseek(fptr, 0, SEEK_END);
            long file_size = ftell(fptr);
            fseek(fptr, 0, SEEK_SET);
            
            // Allocate buffer and read
            json_response = malloc(file_size + 1);
            if (json_response != NULL) {
                fread(json_response, 1, file_size, fptr);
                json_response[file_size] = '\0';
            }
            fclose(fptr);
        }
        
        if (json_response == NULL) {
            printf("Failed to read cache, fetching new data\n");
            cache_status = DOES_NOT_EXIST; // Force fetch
        }
    }
    
    if (cache_status != UP_TO_DATE) {
        // Fetch new data
        printf("Fetching new weather data for: %s, %s\n",
               weather_data->latitude, weather_data->longitude);
        
        if (HTTPClient_GetWeatherData(weather_data) != 0) {
            printf("Failed to fetch weather data\n");
            WeatherData_FreeWeatherData(weather_data);
            return NULL;
        }
        
        // Convert to JSON
        json_response = WeatherData_WeatherToJson(weather_data);
        
        if (json_response != NULL) {
            // Save to cache
            if (Cache_SaveData(&allData, "weather") != 0) {
                printf("Warning: Failed to save to cache\n");
            }
        }
    }
    
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