#include "HTTPResponse.h"
#include "HTTPServerConnection.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Status codes for mapping to text
static const char* HTTPResponse_GetStatusText(HTTPStatusCode _StatusCode)
{
    switch (_StatusCode) {
        case STATUS_OK:
            return "200 OK";
        case STATUS_BAD_REQUEST:
            return "400 Bad Request";
        case STATUS_NOT_FOUND:
            return "404 Not Found";
        case STATUS_INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
        default:
            return "500 Internal Server Error";
    }
}

int HTTPResponse_Initiate(HTTPResponse* _Response, HTTPStatusCode _StatusCode, const char* _ContentType, const char* _Body, size_t _BodyLength)
{
    if (_Response == NULL || _ContentType == NULL) {
        return -1;
    }

    _Response->statusCode = _StatusCode;
    _Response->contentType = _ContentType;
    _Response->body = _Body;
    _Response->bodyLength = _BodyLength;

    return 0;
}


int HTTPResponse_SetResponse(HTTPServerConnection* _Connection, const HTTPResponse* _Response)
{
    if (_Connection == NULL || _Response == NULL) {
        return -1;
    }

    // Build HTTP header
    char header[512];
    int headerLen = snprintf(header, sizeof(header),
                              "HTTP/1.1 %s\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %zu\r\n"
                              "\r\n",
                              HTTPResponse_GetStatusText(_Response->statusCode),
                              _Response->contentType,
                              _Response->bodyLength);

    if (headerLen <= 0 || headerLen >= sizeof(header)) {
        return -1;
    }

    // Calculate total response size (header + body)
    size_t totalSize = headerLen + _Response->bodyLength;

    // Allocate buffer for complete response
    char* responseBuffer = malloc(totalSize);
    if (responseBuffer == NULL) {
        return -1;
    }

    // Copy header to buffer
    memcpy(responseBuffer, header, headerLen);

    // Copy body to buffer if present
    if (_Response->body != NULL && _Response->bodyLength > 0) {
        memcpy(responseBuffer + headerLen, _Response->body, _Response->bodyLength);
    }

    // Set the response on the connection and transfer ownership to the response buffer
    HTTPServerConnection_SetResponse(_Connection, responseBuffer, totalSize);

    // Free our temporary buffer
    free(responseBuffer);

    return 0;
}

int HTTPResponse_SetErrorResponse(HTTPServerConnection* _Connection, HTTPStatusCode _StatusCode, const char* _ErrorMessage)
{
    if (_Connection == NULL || _ErrorMessage == NULL) {
        return -1;
    }

    // Format JSON error body
    char jsonBody[256];
    int jsonLen = snprintf(jsonBody, sizeof(jsonBody),
                           "{\"error\":\"%s\"}",
                           _ErrorMessage);

    if (jsonLen <= 0 || jsonLen >= sizeof(jsonBody)) {
        return -1;
    }

    // Create and set response
    HTTPResponse response;
    HTTPResponse_Initiate(&response, _StatusCode, "application/json", jsonBody, jsonLen);

    return HTTPResponse_SetResponse(_Connection, &response);
}

int HTTPResponse_SetJsonResponse(HTTPServerConnection* _Connection, const char* _JsonBody)
{
    if (_Connection == NULL || _JsonBody == NULL) {
        return -1;
    }

    size_t jsonLen = strlen(_JsonBody);

    HTTPResponse response;
    HTTPResponse_Initiate(&response, STATUS_OK, "application/json", _JsonBody, jsonLen);

    return HTTPResponse_SetResponse(_Connection, &response);
}