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
    ROUTE_INTERNAL_ERROR = -3
} RouteResult;

// Handle incoming HTTP request and send appropriate response to client
RouteResult WeatherRequest_HandleRequest(HTTPServerConnection* _Connection);

typedef struct {
    char method[16];        // GET, POST, etc.
    char path[256];         // /api/v1/weather
    char query_string[512]; // lat=59.3293&lon=18.0686
    char version[16];       // HTTP/1.1
} HTTPRequest;

// Handle geo request (/api/v1/geo?city=...)
// Returns allocated JSON string on success, NULL on error.
// Caller must free returned string.
char* WeatherRequest_HandleGeoRequest(const char* _Url);

// Handle weather request (/api/v1/weather?lat=...&lon=...)
// Returns allocated JSON string on success, NULL on error.
// Caller must free returned string.
char* WeatherRequest_HandleWeatherRequest(const char* _Url);

#endif // __WeatherRequest_h_
