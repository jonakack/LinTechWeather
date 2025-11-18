#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "HTTPClient.h"
#include "WeatherData.h"
#include "utils.h"

GeoData *WeatherData_ParseGeoRequest(const char *_Url)
{
    if (strncmp(_Url, "/api/v1/geo", 11) != 0)
    {
        return NULL;
    }

    // Allocate GeoData structure
    GeoData *data = (GeoData *)calloc(0, sizeof(GeoData));
    if (!data)
        return NULL;

    // Get city parameter
    data->city = get_query_param(_Url, "city");
    if (!data->city)
    {
        WeatherData_FreeGeoData(data);
        return NULL;
    }

    HTTPClient_GetGeoData(data);

    return data;
}

WeatherData *WeatherData_ParseWeatherRequest(const char *_Url)
{
    // Check if URL starts with weather API endpoint
    if (strncmp(_Url, "/api/v1/weather", 14) != 0)
    {
        return NULL;
    }

    // Allocate WeatherData structure
    WeatherData *data = (WeatherData *)calloc(0, sizeof(WeatherData));
    if (!data)
        return NULL;

    // Get lat and lon parameters
    data->latitude = get_query_param(_Url, "lat");
    data->longitude = get_query_param(_Url, "lon");

    HTTPClient_GetWeatherData(data);

    return data;
}

// Shared function to parse HTTP response to clean JSON
static char *WeatherData_HttpResponseToJson(const char *response)
{
    if (!response)
        return NULL;

    // Find the end of HTTP headers
    char *body_start = strstr(response, "\r\n\r\n");
    if (!body_start)
    {
        // No headers found, assume the entire response is the body
        body_start = (char*)response;
    }
    else
    {
        // Skip past \r\n\r\n
        body_start += 4;
    }

    // Check if this is chunked encoding by looking for hex digits followed by newline
    char *first_newline = strchr(body_start, '\n');
    if (first_newline && first_newline > body_start)
    {
        // Check if the line before newline contains only hex digits (chunk size)
        char *line_start = body_start;
        int is_chunked = 1;
        
        // Skip any \r at the start
        while (*line_start == '\r') line_start++;
        
        // Check if we have hex digits
        for (char *p = line_start; p < first_newline && *p != '\r'; p++)
        {
            if (!((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
            {
                is_chunked = 0;
                break;
            }
        }
        
        if (is_chunked && line_start < first_newline)
        {
            // This is chunked encoding - skip the chunk size line
            char *json_start = first_newline + 1;
            
            // Find the end of JSON data (before the final "0" chunk)
            char *json_end = strstr(json_start, "\r\n0\r\n");
            if (!json_end)
            {
                json_end = strstr(json_start, "\n0\n");
            }
            if (!json_end)
            {
                json_end = strstr(json_start, "\n0\r\n");
            }
            
            // If we can't find the end marker, use the rest of the string
            if (!json_end)
            {
                json_end = json_start + strlen(json_start);
            }
            
            // Calculate JSON length
            size_t json_len = json_end - json_start;
            
            // Remove trailing whitespace/newlines
            while (json_len > 0 && (json_start[json_len-1] == '\r' || json_start[json_len-1] == '\n'))
            {
                json_len--;
            }
            
            // Allocate and copy clean JSON
            char *json = (char *)malloc(json_len + 1);
            if (!json)
                return NULL;
            
            strncpy(json, json_start, json_len);
            json[json_len] = '\0';
            
            return json;
        }
    }
    
    // Not chunked encoding - treat as regular response
    size_t body_len = strlen(body_start);
    char *json = (char *)malloc(body_len + 1);
    if (!json)
        return NULL;

    strcpy(json, body_start);
    return json;
}

char *WeatherData_GeoToJson(const GeoData *_Data)
{
    if (!_Data || !_Data->response)
        return NULL;

    return WeatherData_HttpResponseToJson(_Data->response);
}

char *WeatherData_WeatherToJson(const WeatherData *_Data)
{
    if (!_Data || !_Data->response)
        return NULL;

    return WeatherData_HttpResponseToJson(_Data->response);
}

void WeatherData_FreeGeoData(GeoData *_Data)
{
    if (!_Data)
        return;

    // Free all variables in struct
    if (_Data->city)
        free(_Data->city);
    if (_Data->response)
        free(_Data->response);

    free(_Data);
}

void WeatherData_FreeWeatherData(WeatherData *_Data)
{
    if (!_Data)
        return;

    // Free all variables in struct
    if (_Data->latitude)
        free(_Data->latitude);
    if (_Data->longitude)
        free(_Data->longitude);
    if (_Data->response)
        free(_Data->response);

    free(_Data);
}