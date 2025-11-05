#include "RequestRouter.h"
#include "WeatherRequestHandler.h"
#include "HTTPResponse.h"
#include <string.h>
#include <stdlib.h>

// Check if URL matches a specific path
static int RequestRouter_PathMatches(const char* _Url, const char* _Path)
{
    if (_Url == NULL || _Path == NULL) {
        return 0;
    }

    size_t path_len = strlen(_Path);

    // URL must start with path
    if (strncmp(_Url, _Path, path_len) != 0) {
        return 0;
    }

    // After path must be end of string, '?' or other separator
    char next_char = _Url[path_len];
    return (next_char == '\0' || next_char == '?' || next_char == '#');
}

RouteResult RequestRouter_HandleRequest(HTTPServerConnection* _Connection)
{
    if (_Connection == NULL || _Connection->url == NULL) {
        return ROUTE_INTERNAL_ERROR;
    }

    const char* url = _Connection->url;
    char* json_response = NULL;

    // Route to appropriate handler based on URL path
    if (RequestRouter_PathMatches(url, "/api/v1/geo")) {
        json_response = WeatherRequestHandler_HandleGeoRequest(url);
    }
    else if (RequestRouter_PathMatches(url, "/api/v1/weather")) {
        json_response = WeatherRequestHandler_HandleWeatherRequest(url);
    }
    else {
        // Unknown endpoint
        HTTPResponse_SendError(&_Connection->tcpClient,
                              HTTP_STATUS_404_NOT_FOUND,
                              "Endpoint not found");
        return ROUTE_NOT_FOUND;
    }

    // Check if we got a valid JSON response
    if (json_response == NULL) {
        HTTPResponse_SendError(&_Connection->tcpClient,
                              HTTP_STATUS_400_BAD_REQUEST,
                              "Invalid request parameters");
        return ROUTE_BAD_REQUEST;
    }

    // Send successful JSON response
    int send_result = HTTPResponse_SendJson(&_Connection->tcpClient, json_response);

    // Free JSON response
    free(json_response);

    if (send_result < 0) {
        return ROUTE_INTERNAL_ERROR;
    }

    return ROUTE_SUCCESS;
}
