#ifndef __WeatherRequestRouter_h_
#define __WeatherRequestRouter_h_

#include "HTTPServerConnection.h"

// Routing result codes
typedef enum {
    ROUTE_SUCCESS = 0,
    ROUTE_NOT_FOUND = -1,
    ROUTE_BAD_REQUEST = -2,
    ROUTE_INTERNAL_ERROR = -3
} RouteResult;

// Handle incoming HTTP request and send appropriate response to client
RouteResult WeatherRequestRouter_HandleRequest(HTTPServerConnection* _Connection);

#endif // __WeatherRequestRouter_h_
