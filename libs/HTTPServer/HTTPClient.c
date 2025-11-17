#include "TCPClient.h"
#include "HTTPClient.h"

    // https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f&current_weather=true

    // https://geocoding-api.open-meteo.com/v1/search?name=%s

// If path is /api/v1/geo
int HTTPClient_GetGeo(GeoData* _Data)
{
    char request[512];
    snprintf(request, sizeof(request),
             "GET /v1/search?name=%s HTTP/1.1\r\n"
             "Host: geocoding-api.open-meteo.com\r\n"
             "Connection: close\r\n"
             "User-Agent: TCPClient/1.0\r\n"
             "\r\n", _Data->city);

    // TODO: Byt ut mot gemensam HTTPClient_Get funktion

    TCPClient client;
    
    TCPClient_Initiate(&client, -1);  // Initialize with -1
    printf("Client fd: %d\n", client.fd);
    
    if (TCPClient_Connect(&client, "geocoding-api.open-meteo.com", "80") != 0) {
        printf("Failed to connect\n");
        return -1;
    }
    printf("Client fd: %d\n", client.fd);
    
    printf("Sending GET request\n");
    int sent = TCPClient_Write(&client, (uint8_t*)request, strlen(request));
    if (sent < 0) {
        printf("Failed to send request\n");
        TCPClient_Dispose(&client);
        return -1;
    }
                    
    printf("Request sent (%d bytes)\n", sent);
    // sleep(5); 
    // TODO: READ BYTES

    // TODO: Remove HTTP response headers

    char response[10240];
    int bytes_received = TCPClient_Read(&client, (uint8_t*)response, sizeof(response) - 1);
    
    if (bytes_received > 0) {
        response[bytes_received] = '\0';  // Proper null termination
        printf("Printing response (%d bytes):\n%s\n", bytes_received, response);
    } else {
        printf("No data received or error\n");
    }

    _Data->response = response;
    
    TCPClient_Dispose(&client);
    return 0;
}

// If path is /api/v1/weather
// int HTTPClient_GetWeather(double _Lat, double _Lon)
// {
//     char request[] = "GET /v1/forecast?latitude=%.4f&longitude=%.4f&current_weather=true HTTPS/1.1\r\n"
//                      "Host: api.open-meteo.com\r\n"
//                      "Connection: close\r\n"
//                      "User-Agent: TCPClient/1.0\r\n"
//                      "\r\n", _Lat, _Lon;

//     // HTTPClient_Get(request);

//     return 0;
// }

// int HTTPClient_Get(char *_Host, char *_Port, char *_Request)

//     TCPClient client;
    
//     TCPClient_Initiate(&client, -1);  // Initialize with -1
//     printf("Client fd: %d\n", client.fd);
    
//     if (TCPClient_Connect(&client, "kontoret.onvo.se", "10380") != 0) {
//         printf("Failed to connect\n");
//         return -1;
//     }
//     printf("Client fd: %d\n", client.fd);
    
//     printf("Sending GET request\n");
//     int sent = TCPClient_Write(&client, (uint8_t*)request, strlen(request));
//     if (sent < 0) {
//         printf("Failed to send request\n");
//         TCPClient_Dispose(&client);
//         return -1;
//     }
                    
//     printf("Request sent (%d bytes)\n", sent);
//     sleep(1);

//     char response[2048];
//     int bytes_received = TCPClient_Read(&client, (uint8_t*)response, sizeof(response) - 1);
    
//     if (bytes_received > 0) {
//         response[bytes_received] = '\0';  // Proper null termination
//         printf("Printing response (%d bytes):\n%s\n", bytes_received, response);
//     } else {
//         printf("No data received or error\n");
//     }
    
//     TCPClient_Dispose(&client);