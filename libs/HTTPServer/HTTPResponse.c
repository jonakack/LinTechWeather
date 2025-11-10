#include "HTTPResponse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Convert status code to HTTP status text
static const char* HTTPResponse_GetStatusText(HTTPStatusCode _StatusCode)
{
    switch (_StatusCode) {
        case HTTP_STATUS_200_OK:
            return "200 OK";
        case HTTP_STATUS_400_BAD_REQUEST:
            return "400 Bad Request";
        case HTTP_STATUS_404_NOT_FOUND:
            return "404 Not Found";
        case HTTP_STATUS_500_INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
        default:
            return "500 Internal Server Error";
    }
}

int HTTPResponse_Initiate(HTTPResponse* _Response,
                          HTTPStatusCode _StatusCode,
                          const char* _ContentType,
                          const char* _Body,
                          size_t _BodyLength)
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

int HTTPResponse_Send(const HTTPResponse* _Response, TCPClient* _TcpClient)
{
    if (_Response == NULL || _TcpClient == NULL) {
        return -1;
    }

    // Build HTTP header
    char header[512];
    int header_len = snprintf(header, sizeof(header),
                              "HTTP/1.1 %s\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %zu\r\n"
                              "\r\n",
                              HTTPResponse_GetStatusText(_Response->statusCode),
                              _Response->contentType,
                              _Response->bodyLength);

    if (header_len <= 0 || header_len >= sizeof(header)) {
        return -1;
    }

    // Send header
    int result = TCPClient_Write(_TcpClient, (uint8_t*)header, header_len);
    if (result < 0) {
        return result;
    }

    // Send body if present
    if (_Response->body != NULL && _Response->bodyLength > 0) {
        result = TCPClient_Write(_TcpClient, (uint8_t*)_Response->body, (int)_Response->bodyLength);
        if (result < 0) {
            return result;
        }
    }

    return 0;
}

int HTTPResponse_SendError(TCPClient* _TcpClient,
                           HTTPStatusCode _StatusCode,
                           const char* _ErrorMessage)
{
    if (_TcpClient == NULL || _ErrorMessage == NULL) {
        return -1;
    }

    // Format JSON error body
    char json_body[256];
    int json_len = snprintf(json_body, sizeof(json_body),
                           "{\"error\":\"%s\"}",
                           _ErrorMessage);

    if (json_len <= 0 || json_len >= sizeof(json_body)) {
        return -1;
    }

    // Create and send response
    HTTPResponse response;
    HTTPResponse_Initiate(&response, _StatusCode, "application/json", json_body, json_len);

    return HTTPResponse_Send(&response, _TcpClient);
}

int HTTPResponse_SendJson(TCPClient* _TcpClient, const char* _JsonBody)
{
    if (_TcpClient == NULL || _JsonBody == NULL) {
        return -1;
    }

    size_t json_len = strlen(_JsonBody);

    HTTPResponse response;
    HTTPResponse_Initiate(&response, HTTP_STATUS_200_OK, "application/json", _JsonBody, json_len);

    return HTTPResponse_Send(&response, _TcpClient);
}