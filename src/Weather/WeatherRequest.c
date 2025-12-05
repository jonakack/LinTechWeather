#include "WeatherRequest.h"
#include "TaskScheduler.h"
#include "Utils.h"

RouteResult WeatherRequest_HandleRequest(HTTPServerConnection* _Connection)
{
    if (_Connection == NULL || _Connection->url == NULL) {
        return ROUTE_INTERNAL_ERROR;
    }

    const char* url = _Connection->url;

    // Here we determine which endpoint is being requested
    RouteResult result; // To hold the result of the handler

    // Route to appropriate handler based on URL path
    if (compare_url(url, "/api/v1/geo")) {
        result = WeatherRequest_HandleGeoRequest(_Connection, url);
    }
    else if (compare_url(url, "/api/v1/weather")) {
        result = WeatherRequest_HandleWeatherRequest(_Connection, url);
    }
    else {
        // Unknown endpoint
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_NOT_FOUND,
                                     "Endpoint not found");
        return ROUTE_NOT_FOUND;
    }

    // ROUTE_SUCCESS = cache hit, response already set
    // ROUTE_PENDING = async request started, will call back later
    // Other = error, error response already set
    return result;
}

RouteResult WeatherRequest_HandleGeoRequest(HTTPServerConnection* _Connection, const char* _Url)
{
    if (_Connection == NULL || _Url == NULL)
    {
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_BAD_REQUEST,
                                     "Invalid request parameters");
        return ROUTE_BAD_REQUEST;
    }

    // Parse geo request from URL
    WeatherData* geoData = WeatherData_ParseRequest(_Url);
    if (geoData == NULL)
    {
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_BAD_REQUEST,
                                     "Invalid request parameters");
        return ROUTE_BAD_REQUEST;
    }

    // Initialize cache configuration
    CacheConfig geoCache =
    {
        .base_path = "./libs/WeatherServer/cache/geodata",
        .ttl_seconds = 900  // 15 minutes
    };

    int cacheStatus = Cache_Check(geoData->city, &geoCache);

    // Always use cached data if available, since cities don't move
    if (cacheStatus != DOES_NOT_EXIST)
    {
        // Convert to JSON from cached file
        char* jsonResponse = Cache_Load(geoData->city, &geoCache);
        if (jsonResponse != NULL)
        {
            // Cache hit - send response immediately
            HTTPResponse_SetJsonResponse(_Connection, jsonResponse);
            free(jsonResponse);
            WeatherData_Dispose(geoData);
            return ROUTE_SUCCESS;
        }
        printf("Failed to read cache, fetching new data\n");
    }

    // Cache miss - start async request
    printf("Fetching new geo data for: %s\n", geoData->city);

    // Create async context
    WeatherRequestContext* context = malloc(sizeof(WeatherRequestContext));
    if (context == NULL) 
    {
        WeatherData_Dispose(geoData);
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_INTERNAL_SERVER_ERROR,
                                     "Internal server error");
        return ROUTE_INTERNAL_ERROR;
    }

    context->connection = _Connection;
    context->cacheConfig = geoCache; 
    strncpy(context->cacheKey, geoData->city, sizeof(context->cacheKey) - 1);
    context->cacheKey[sizeof(context->cacheKey) - 1] = '\0';

    // Store context in HTTPClient (we'll use a custom wrapper)
    HTTPClient* client = malloc(sizeof(HTTPClient));
    if (client == NULL)
    {
        free(context);
        WeatherData_Dispose(geoData);
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_INTERNAL_SERVER_ERROR,
                                     "Internal server error");
        return ROUTE_INTERNAL_ERROR;
    }

    HTTPClient_Initiate(client, _Connection->scheduler);
    strcpy(client->host, "geocoding-api.open-meteo.com");
    strcpy(client->port, "80");
    client->weather_data = geoData;

    // Store context in weather data so callback can access it
    geoData->context = context;

    char url[256];
    snprintf(url, sizeof(url), "/v1/search?name=%s", geoData->city);

    if (HTTPClient_GET(client, url, WeatherRequest_OnGeoDataComplete) != 0) 
    {
        free(context);
        free(client);
        WeatherData_Dispose(geoData);
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_INTERNAL_SERVER_ERROR,
                                     "Failed to start request");
        return ROUTE_INTERNAL_ERROR;
    }

    // Request started successfully - response will be sent by callback
    return ROUTE_PENDING;
}

RouteResult WeatherRequest_HandleWeatherRequest(HTTPServerConnection* _Connection, const char* _Url)
{
    if (_Connection == NULL || _Url == NULL)
    {
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_BAD_REQUEST,
                                     "Invalid request parameters");
        return ROUTE_BAD_REQUEST;
    }

    WeatherData* weatherData = WeatherData_ParseRequest(_Url);
    if (weatherData == NULL)
    {
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_BAD_REQUEST,
                                     "Invalid request parameters");
        return ROUTE_BAD_REQUEST;
    }

    // Initialize cache configuration
    CacheConfig weatherCache =
    {
        .base_path = "./libs/WeatherServer/cache/weatherdata",
        .ttl_seconds = 900  // 15 minutes
    };

    char cacheKey[256];
    snprintf(cacheKey, sizeof(cacheKey), "%s_%s",
             weatherData->latitude, weatherData->longitude);

    int cacheStatus = Cache_Check(cacheKey, &weatherCache);

    if (cacheStatus == UP_TO_DATE)
    {
        char* jsonResponse = Cache_Load(cacheKey, &weatherCache);
        if (jsonResponse != NULL)
        {
            // Cache hit - send response immediately
            HTTPResponse_SetJsonResponse(_Connection, jsonResponse);
            free(jsonResponse);
            WeatherData_Dispose(weatherData);
            return ROUTE_SUCCESS;
        }
        printf("Failed to read cache, fetching new data\n");
    }

    // Cache miss - start async request
    printf("Fetching new weather data for: %s, %s\n",
           weatherData->latitude, weatherData->longitude);

    // Create async context
    WeatherRequestContext* context = malloc(sizeof(WeatherRequestContext));
    if (context == NULL) 
    {
        WeatherData_Dispose(weatherData);
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_INTERNAL_SERVER_ERROR,
                                     "Internal server error");
        return ROUTE_INTERNAL_ERROR;
    }

    context->connection = _Connection;
    context->cacheConfig = weatherCache;
    strncpy(context->cacheKey, cacheKey, sizeof(context->cacheKey) - 1);
    context->cacheKey[sizeof(context->cacheKey) - 1] = '\0';

    // Store context in HTTPClient
    HTTPClient* client = malloc(sizeof(HTTPClient));
    if (client == NULL)
    {
        free(context);
        WeatherData_Dispose(weatherData);
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_INTERNAL_SERVER_ERROR,
                                     "Internal server error");
        return ROUTE_INTERNAL_ERROR;
    }

    HTTPClient_Initiate(client, _Connection->scheduler);
    strcpy(client->host, "api.open-meteo.com");
    strcpy(client->port, "80");
    client->weather_data = weatherData;
    weatherData->context = context;

    char url[256];
    snprintf(url, sizeof(url), "/v1/forecast?latitude=%s&longitude=%s&current_weather=true",
             weatherData->latitude, weatherData->longitude);

    if (HTTPClient_GET(client, url, WeatherRequest_OnWeatherDataComplete) != 0) 
    {
        free(context);
        free(client);
        WeatherData_Dispose(weatherData);
        HTTPResponse_SetErrorResponse(_Connection,
                                     STATUS_INTERNAL_SERVER_ERROR,
                                     "Failed to start request");
        return ROUTE_INTERNAL_ERROR;
    }

    // Request started successfully - response will be sent by callback
    return ROUTE_PENDING;
}

// Async callback for geo data completion
void WeatherRequest_OnGeoDataComplete(HTTPClient* _Client, const char* _Event)
{
    if (strcmp(_Event, "complete") != 0 || _Client == NULL) {
        return;
    }

    WeatherData* geoData = _Client->weather_data;
    if (geoData == NULL || geoData->context == NULL) 
    {
        // Something went wrong - cleanup what we can
        // HTTPClient task and buffer will be cleaned up by state machine
        return;
    }

    WeatherRequestContext* context = (WeatherRequestContext*)geoData->context;

    // Convert HTTP response to JSON
    char* jsonResponse = WeatherData_HttpResponseToJson((char*)_Client->buffer);

    if (jsonResponse != NULL) 
    {
        // Save to cache
        if (Cache_Save(context->cacheKey, jsonResponse, &context->cacheConfig) != 0) 
        {
            printf("Warning: Failed to save geo data to cache\n");
        }

        // Set response on connection
        HTTPResponse_SetJsonResponse(context->connection, jsonResponse);
        free(jsonResponse);
    } else 
    {
        // Failed to parse response
        HTTPResponse_SetErrorResponse(context->connection,
                                     STATUS_INTERNAL_SERVER_ERROR,
                                     "Failed to process geo data");
    }

    // Cleanup our allocated resources
    free(context);
    WeatherData_Dispose(geoData);

    // Cleanup HTTPClient - we do it here to avoid state machine trying to do it
    // Destroy task
    if (_Client->task != NULL)
    {
        TaskScheduler_DestroyTask(_Client->scheduler, _Client->task);
        _Client->task = NULL;  // Prevent double-destroy in Close state
    }

    // Free buffer
    if (_Client->buffer != NULL) 
    {
        free(_Client->buffer);
        _Client->buffer = NULL;
    }

    // Disconnect TCP
    TCPClient_Disconnect(&_Client->tcpClient);

    // Free HTTPClient structure itself
    free(_Client);
}

// Async callback for weather data completion
void WeatherRequest_OnWeatherDataComplete(HTTPClient* _Client, const char* _Event)
{
    if (strcmp(_Event, "complete") != 0 || _Client == NULL) 
    {
        return;
    }

    WeatherData* weatherData = _Client->weather_data;
    if (weatherData == NULL || weatherData->context == NULL) 
    {
        // Something went wrong - cleanup what we can
        // HTTPClient task and buffer will be cleaned up by state machine
        return;
    }

    WeatherRequestContext* context = (WeatherRequestContext*)weatherData->context;

    // Convert HTTP response to JSON
    char* jsonResponse = WeatherData_HttpResponseToJson((char*)_Client->buffer);

    if (jsonResponse != NULL) 
    {
        // Save to cache
        if (Cache_Save(context->cacheKey, jsonResponse, &context->cacheConfig) != 0) 
        {
            printf("Warning: Failed to save weather data to cache\n");
        }

        // Set response on connection
        HTTPResponse_SetJsonResponse(context->connection, jsonResponse);
        free(jsonResponse);
    } else {
        // Failed to parse response
        HTTPResponse_SetErrorResponse(context->connection,
                                     STATUS_INTERNAL_SERVER_ERROR,
                                     "Failed to process weather data");
    }

    // Cleanup our allocated resources
    free(context);
    WeatherData_Dispose(weatherData);

    // Cleanup HTTPClient - we do it here to avoid state machine trying to do it
    // Destroy task
    if (_Client->task != NULL)
    {
        TaskScheduler_DestroyTask(_Client->scheduler, _Client->task);
        _Client->task = NULL;  // Prevent double-destroy in Close state
    }

    // Free buffer
    if (_Client->buffer != NULL)
    {
        free(_Client->buffer);
        _Client->buffer = NULL;
    }

    // Disconnect TCP
    TCPClient_Disconnect(&_Client->tcpClient);

    // Free HTTPClient structure itself
    free(_Client);
}

int WeatherRequest_ParseHTTPRequest(const char* _RequestLine, HTTPRequest* _ParsedRequest)
{
    if (!_RequestLine || !_ParsedRequest) 
    {
        return -1;
    }
    
    // Initialize the struct
    memset(_ParsedRequest, 0, sizeof(HTTPRequest));
    
    // Copy request line for parsing (strtok modifies the string)
    char request_copy[1024];
    strncpy(request_copy, _RequestLine, sizeof(request_copy) - 1);
    request_copy[sizeof(request_copy) - 1] = '\0';
    
    // Parse: "GET /api/v1/weather?lat=59.3293&lon=18.0686 HTTP/1.1"
    char* token;
    
    // Get method (GET, POST, etc.)
    token = strtok(request_copy, " ");
    if (token) 
    {
        strncpy(_ParsedRequest->method, token, sizeof(_ParsedRequest->method) - 1);
    }
    
    // Get URL (path + query string)
    token = strtok(NULL, " ");
    if (token) 
    {
        // Split path and query string at '?'
        char* query_start = strchr(token, '?');
        if (query_start) 
        {
            // Copy path (everything before '?')
            size_t path_len = query_start - token;
            strncpy(_ParsedRequest->path, token, 
                   path_len < sizeof(_ParsedRequest->path) - 1 ? path_len : sizeof(_ParsedRequest->path) - 1);
            
            // Copy query string (everything after '?')
            strncpy(_ParsedRequest->query_string, query_start + 1, sizeof(_ParsedRequest->query_string) - 1);
        } else {
            // No query string, just path
            strncpy(_ParsedRequest->path, token, sizeof(_ParsedRequest->path) - 1);
        }
    }
    
    // Get HTTP version
    token = strtok(NULL, " ");
    if (token) 
    {
        strncpy(_ParsedRequest->version, token, sizeof(_ParsedRequest->version) - 1);
    }

    return 0; // Success
}