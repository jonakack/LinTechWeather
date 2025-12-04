#ifndef __WeatherRequest_h_
#define __WeatherRequest_h_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "HTTPResponse.h"
#include "HTTPServerConnection.h"
#include "WeatherData.h"
#include "HTTPClient.h"
#include "cache.h"
#include "utils.h"

// Routing result codes
typedef enum {
    ROUTE_SUCCESS = 0,
    ROUTE_NOT_FOUND = -1,
    ROUTE_BAD_REQUEST = -2,
    ROUTE_INTERNAL_ERROR = -3,
    ROUTE_PENDING = 1  // Request is being processed asynchronously
} RouteResult;

// Async context for weather requests
typedef struct {
    HTTPServerConnection* connection;
    CacheConfig cacheConfig;
    char cacheKey[256];
} WeatherRequestContext;

// Handle incoming HTTP request and send appropriate response to client
RouteResult WeatherRequest_HandleRequest(HTTPServerConnection* _Connection);

typedef struct {
    char method[16];        // GET, POST, etc.
    char path[256];         // /api/v1/weather
    char query_string[512]; // lat=59.3293&lon=18.0686
    char version[16];       // HTTP/1.1
} HTTPRequest;

// Handle geo request async (/api/v1/geo?city=...)
// Returns ROUTE_SUCCESS if cache hit, ROUTE_PENDING if async request started
RouteResult WeatherRequest_HandleGeoRequest(HTTPServerConnection* _Connection, const char* _Url);

// Handle weather request async (/api/v1/weather?lat=...&lon=...)
// Returns ROUTE_SUCCESS if cache hit, ROUTE_PENDING if async request started
RouteResult WeatherRequest_HandleWeatherRequest(HTTPServerConnection* _Connection, const char* _Url);

// Async callbacks (internal use)
void WeatherRequest_OnGeoDataComplete(HTTPClient* _Client, const char* _Event);
void WeatherRequest_OnWeatherDataComplete(HTTPClient* _Client, const char* _Event);

#endif // __WeatherRequest_h_
