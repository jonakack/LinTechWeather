#ifndef __HTTPResponse_h_
#define __HTTPResponse_h_

#include <stdint.h>
#include <stddef.h>
#include "HTTPServerConnection.h"

// HTTP status codes
typedef enum {
    STATUS_OK = 200,
    STATUS_BAD_REQUEST = 400,
    STATUS_NOT_FOUND = 404,
    STATUS_INTERNAL_SERVER_ERROR = 500
} HTTPStatusCode;

// HTTP response structure
typedef struct {
    HTTPStatusCode statusCode;
    const char* contentType;
    const char* body;
    size_t bodyLength;
} HTTPResponse;

// Initiate the response struct 
int HTTPResponse_Initiate(HTTPResponse* _Response, HTTPStatusCode _StatusCode, const char* _ContentType, const char* _Body, size_t _BodyLength);

// Set HTTP response on connection (-1 on error 0 on success)
int HTTPResponse_SetResponse(HTTPServerConnection* _Connection, const HTTPResponse* _Response);

// Set JSON response on connection (-1 on error 0 on success)
int HTTPResponse_SetJsonResponse(HTTPServerConnection* _Connection, const char* _JsonBody);

// Set JSON error response on connection (-1 on error 0 on success) 
int HTTPResponse_SetErrorResponse(HTTPServerConnection* _Connection, HTTPStatusCode _StatusCode, const char* _ErrorMessage);


#endif // __HTTPResponse_h_