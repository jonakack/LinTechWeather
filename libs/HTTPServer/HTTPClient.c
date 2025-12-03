#include "HTTPClient.h"
#include "TCPClient.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// ---------------------------------- Templates --------------------------------------------
//
// https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f&current_weather=true
// https://geocoding-api.open-meteo.com/v1/search?name=%s
//
// -----------------------------------------------------------------------------------------


void HTTPClient_Work(void* _Context, uint64_t _MonTime)
{
    HTTPClient* client = (HTTPClient*)_Context;
    
    switch (client->state) {
        case HTTPClient_State_Init:
            // Initialize TCP connection
            client->state = HTTPClient_State_Connect;
            break;
            
        case HTTPClient_State_Connect:
            printf("HTTPClient: Attempting to connect to %s:%s\n", client->host, client->port);
            TCPClient_Initiate(&client->tcpClient, -1);
            if (TCPClient_Connect(&client->tcpClient, client->host, client->port) == 0) {
                // printf("HTTPClient: Connected successfully\n");
                client->state = HTTPClient_State_Transmit;
                client->bytes_sent = 0;
            } else {
                printf("HTTPClient: Connection failed, retrying...\n");
            }
            break;
            
        case HTTPClient_State_Transmit:
            // printf("HTTPClient: Sending request...\n");
            int request_len = strlen((char*)client->buffer);
            int sent = TCPClient_Write(&client->tcpClient, client->buffer + client->bytes_sent, 
                                     request_len - client->bytes_sent);
            
            if (sent > 0) {
                client->bytes_sent += sent;
                // printf("HTTPClient: Sent %d bytes (%d/%d total)\n", sent, client->bytes_sent, request_len);
                
                if (client->bytes_sent >= request_len) {
                    // printf("HTTPClient: Request sent completely\n");
                    client->state = HTTPClient_State_Receive;
                    // Reset buffer for receiving
                    memset(client->buffer, 0, 4096);
                    client->bufferPtr = client->buffer;
                    client->total_received = 0;
                }
                // If sent < request_len, stay in transmit state to send more
            } else if (sent == 0) {
                printf("HTTPClient: Send returned 0, connection may be closed\n");
                client->state = HTTPClient_State_Close;
            } else {
                // sent < 0 - could be EAGAIN/EWOULDBLOCK or real error
                // For now, just continue trying - don't treat as error immediately
                // printf("HTTPClient: Send would block, trying again...\n");
            }
            break;
            
        case HTTPClient_State_Receive:
            int available_space = 4095 - client->total_received; // Leave space for null terminator
            
            if (available_space > 0) {
                int received = TCPClient_Read(&client->tcpClient, 
                                            client->buffer + client->total_received,
                                            available_space);
                
                if (received > 0) {
                    client->total_received += received;
                    // printf("HTTPClient: Received %d bytes (total: %d)\n", received, client->total_received);
                    // Continue reading
                } else if (received == 0) {
                    // Connection closed by server
                    printf("HTTPClient: Server closed connection, response complete (%d bytes)\n", client->total_received);
                    client->buffer[client->total_received] = '\0';
                    client->state = HTTPClient_State_Close;
                    
                    // Call callback when done
                    if (client->callback) {
                        client->callback(client, "complete");
                    }
                } else {
                    // received < 0 - could be EAGAIN/EWOULDBLOCK (no data yet) or real error
                    // For now, just continue trying - don't treat as error immediately
                    // printf("HTTPClient: No data available yet, continuing...\n");
                }
            } else {
                printf("HTTPClient: Buffer full (%d bytes)\n", client->total_received);
                client->buffer[4095] = '\0';
                client->state = HTTPClient_State_Close;
                
                // Call callback when done
                if (client->callback) {
                    client->callback(client, "complete");
                }
            }
            break;
            
        case HTTPClient_State_Close:
            printf("HTTPClient: Closing connection\n");
            TCPClient_Disconnect(&client->tcpClient);
            smw_destroyTask(client->task);
            client->task = NULL;
            break;
    }
}

int HTTPClient_Initiate(HTTPClient* _Client)
{
    memset(_Client, 0, sizeof(HTTPClient));
    
    _Client->buffer = NULL;
    _Client->task = NULL;
    _Client->state = HTTPClient_State_Init;
    _Client->bytes_sent = 0;
    _Client->total_received = 0;

    return 0;
}

int HTTPClient_GET(HTTPClient* _Client, const char* _URL, void (*callback)(HTTPClient* _Client, const char* _Event))
{
    _Client->buffer = malloc(4096);
    if(_Client->buffer == NULL)
        return -1;

    _Client->callback = callback;

    snprintf((char*)_Client->buffer, 4096, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", 
             _URL, _Client->host);

    _Client->bufferPtr = _Client->buffer;
    _Client->state = HTTPClient_State_Init;
    _Client->bytes_sent = 0;
    _Client->total_received = 0;

    printf("HTTPClient: Starting request: %s\n", (char*)_Client->buffer);

    _Client->task = smw_createTask(_Client, HTTPClient_Work);
    
    return 0;
}

void HTTPClient_Dispose(HTTPClient* _Client)
{
    if(_Client->task != NULL)
        smw_destroyTask(_Client->task);

    if(_Client->buffer != NULL)
        free(_Client->buffer);
        
    TCPClient_Disconnect(&_Client->tcpClient);
    memset(_Client, 0, sizeof(HTTPClient));
}

// Wrapper for geo data
int HTTPClient_GetGeoData(WeatherData* _Data)
{
    if (_Data == NULL || _Data->city == NULL) {
        return -1;
    }

    HTTPClient* client = malloc(sizeof(HTTPClient));
    if (!client) return -1;
    
    HTTPClient_Initiate(client);
    
    // Set host and store reference to data
    strcpy(client->host, "geocoding-api.open-meteo.com");
    strcpy(client->port, "80");
    client->weather_data = _Data;
    
    char url[256];
    snprintf(url, sizeof(url), "/v1/search?name=%s", _Data->city);
    
    return HTTPClient_GET(client, url, HTTPClient_GeoCallback);
}

// Wrapper for weather data
int HTTPClient_GetWeatherData(WeatherData* _Data)
{
    if (_Data == NULL || _Data->latitude == NULL || _Data->longitude == NULL) {
        return -1;
    }

    HTTPClient* client = malloc(sizeof(HTTPClient));
    if (!client) return -1;
    
    HTTPClient_Initiate(client);
    
    // Set host and store reference to data
    strcpy(client->host, "api.open-meteo.com");
    strcpy(client->port, "80");
    client->weather_data = _Data;
    
    char url[256];
    snprintf(url, sizeof(url), "/v1/forecast?latitude=%s&longitude=%s&current_weather=true", 
             _Data->latitude, _Data->longitude);
    
    return HTTPClient_GET(client, url, HTTPClient_WeatherCallback);
}

// Callback for geo data completion
void HTTPClient_GeoCallback(HTTPClient* _Client, const char* _Event)
{
    if (strcmp(_Event, "complete") == 0) {
        printf("HTTPClient: Geo request completed\n");
        // Copy response to weather data
        size_t len = strlen((char*)_Client->buffer);
        _Client->weather_data->response = malloc(len + 1);
        if (_Client->weather_data->response) {
            strcpy(_Client->weather_data->response, (char*)_Client->buffer);
            printf("HTTPClient: Response copied (%zu bytes)\n", len);
        }
        
        // Mark request as complete
        _Client->weather_data->request_complete = 1;
        
        HTTPClient_Dispose(_Client);
        free(_Client);
    }
}

// Callback for weather data completion  
void HTTPClient_WeatherCallback(HTTPClient* _Client, const char* _Event)
{
    if (strcmp(_Event, "complete") == 0) {
        printf("HTTPClient: Weather request completed\n");
        // Copy response to weather data
        size_t len = strlen((char*)_Client->buffer);
        _Client->weather_data->response = malloc(len + 1);
        if (_Client->weather_data->response) {
            strcpy(_Client->weather_data->response, (char*)_Client->buffer);
            printf("HTTPClient: Response copied (%zu bytes)\n", len);
        }
        
        // Mark request as complete
        _Client->weather_data->request_complete = 1;
        
        HTTPClient_Dispose(_Client);
        free(_Client);
    }
}