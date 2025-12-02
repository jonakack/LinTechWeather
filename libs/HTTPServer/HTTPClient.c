#include <stdlib.h>
#include "TCPClient.h"
#include "HTTPClient.h"
#include "cache.h"

#define PORT "80"

// ---------------------------------- Templates --------------------------------------------
//
// https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f&current_weather=true
// https://geocoding-api.open-meteo.com/v1/search?name=%s
//
// -----------------------------------------------------------------------------------------

// TODO: Error handling
// TODO: Add dynamic port

// Gets a GeoData struct, assembles GET request and sends it to HTTPClient_Get
// When done, it fills the data structure with JSON data from external API
int HTTPClient_GetGeoData(AllData *_AllData)
{
    char request[512];
    snprintf(request, sizeof(request),
             "GET /v1/search?name=%s HTTP/1.1\r\n"
             "Host: geocoding-api.open-meteo.com\r\n"
             "Connection: close\r\n"
             "User-Agent: TCPClient/1.0\r\n"
             "\r\n",
             _AllData->GeoData->city);

    _AllData->GeoData->response = HTTPClient_Get("geocoding-api.open-meteo.com", PORT, request);
    Cache_SaveData(_AllData, "geo");
    

    return 0;
}

// If path is /api/v1/weather
int HTTPClient_GetWeatherData(AllData *_AllData)
{
    char request[512];
    snprintf(request, sizeof(request),
         "GET /v1/forecast?latitude=%s&longitude=%s&current_weather=true HTTP/1.1\r\n"
         "Host: api.open-meteo.com\r\n"
         "Connection: close\r\n"
         "User-Agent: TCPClient/1.0\r\n"
         "\r\n", _AllData->WeatherData->latitude, _AllData->WeatherData->longitude);

    _AllData->WeatherData->response = HTTPClient_Get("api.open-meteo.com", PORT, request);
    Cache_SaveData(_AllData, "weather");

    return 0;
}

// Universal function that sends a GET request to chosen HTTP server. Works for all APIs
// Returns HTTP response
// ! Caller must free response !
char *HTTPClient_Get(char *_Host, char *_Port, char *_Request)
{
    printf("HTTPClient: Running HTTPClient_Get\nHost: %s\nPort: %s\n\nRequest sent:\n%s\n", _Host, _Port, _Request);

    TCPClient client;

    TCPClient_Initiate(&client, -1); // Initialize with -1
    printf("HTTPClient fd: %d\n", client.fd);

    if (TCPClient_Connect(&client, _Host, _Port) != 0)
    {
        printf("HTTPClient: Failed to connect\n");
        return NULL;
    }
    printf("HTTPClient: connected. Fd: %d\n", client.fd);

    printf("HTTPClient: Sending GET request\n");
    int sent = TCPClient_Write(&client, (uint8_t *)_Request, strlen(_Request));
    if (sent < 0)
    {
        printf("HTTPClient: Failed to send request\n");
        TCPClient_Dispose(&client);
        return NULL;
    }

    printf("HTTPClient: Request sent (%d bytes)\n", sent);

    char temp_response[8192];
    int total_bytes = 0;
    int bytes_received = 0;

    // Keep reading until connection closes or buffer is full
    while (1)
    {
        bytes_received = TCPClient_Read(&client, (uint8_t *)(temp_response + total_bytes),
                                        sizeof(temp_response) - total_bytes - 1);

        if (bytes_received > 0)
        {
            // Got data
            total_bytes += bytes_received;
            printf("HTTPClient: Read %d bytes (total: %d)\n", bytes_received, total_bytes);
            if (total_bytes >= sizeof(temp_response) - 1)
                break;
        }
        else if (bytes_received == 0)
        {
            // Connection closed by server - we're done
            break;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // No data available right now, but connection is still open - continue
            continue;
        }
        else
        {
            // Error occurred
            printf("HTTPClient: Read error: %s\n", strerror(errno));
            break;
        }
    }

    if (total_bytes <= 0)
    {
        printf("HTTPClient: No data received or error\n");
        TCPClient_Dispose(&client);
        return NULL;
    }

    // Checks for valid HTTP response
    if (strncmp(temp_response, "HTTP/", 5) != 0)
    {
        printf("HTTPClient: Invalid HTTP response\n");
        TCPClient_Dispose(&client);
        return NULL;
    }

    temp_response[total_bytes] = '\0';

    char *response = malloc(total_bytes + 1);
    if (!response)
    {
        printf("HTTPClient: malloc failed\n");
        TCPClient_Dispose(&client);
        return NULL;
    }
    
    // strcpy(response, temp_response);
    memcpy(response, temp_response, total_bytes);
    response[total_bytes] = '\0';

    // printf("HTTPClient: Printing response (%d bytes):\n%s\n", total_bytes, response);

    TCPClient_Dispose(&client);
    return response;
}