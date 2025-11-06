#ifndef __HTTPResponse_h_
#define __HTTPResponse_h_

#include <stdint.h>
#include <stddef.h>
#include "TCPClient.h"

// HTTP status codes
typedef enum {
    HTTP_STATUS_200_OK = 200,
    HTTP_STATUS_400_BAD_REQUEST = 400,
    HTTP_STATUS_404_NOT_FOUND = 404,
    HTTP_STATUS_500_INTERNAL_SERVER_ERROR = 500
} HTTPStatusCode;

// HTTP response structure
typedef struct {
    HTTPStatusCode statusCode;
    const char* contentType;
    const char* body;
    size_t bodyLength;
} HTTPResponse;

// Initialize response struct.
int HTTPResponse_Initiate(HTTPResponse* _Response,
                          HTTPStatusCode _StatusCode,
                          const char* _ContentType,
                          const char* _Body,
                          size_t _BodyLength);

// Send HTTP response over TCP connection. 
int HTTPResponse_Send(const HTTPResponse* _Response, TCPClient* _TcpClient);

// Send JSON error response with given status code. 
int HTTPResponse_SendError(TCPClient* _TcpClient,
                           HTTPStatusCode _StatusCode,
                           const char* _ErrorMessage);

// Send JSON response with 200 OK status.
int HTTPResponse_SendJson(TCPClient* _TcpClient, const char* _JsonBody);

#endif // __HTTPResponse_h_