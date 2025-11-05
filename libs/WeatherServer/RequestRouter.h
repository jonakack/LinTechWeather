#ifndef __RequestRouter_h_
#define __RequestRouter_h_

#include "HTTPServerConnection.h"

// Routing result codes
typedef enum {
    ROUTE_SUCCESS = 0,
    ROUTE_NOT_FOUND = -1,
    ROUTE_BAD_REQUEST = -2,
    ROUTE_INTERNAL_ERROR = -3
} RouteResult;

// Handle incoming HTTP request and send appropriate response to client
RouteResult RequestRouter_HandleRequest(HTTPServerConnection* _Connection);

#endif // __RequestRouter_h_
