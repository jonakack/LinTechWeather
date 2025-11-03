#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "HTTPClient.h"
#include "TCPClient.h"

int HTTPClient_ParseURL(const char *_URL, ParsedURL *_Result)
{
    // Setting default values
    _Result->port = 80;  // Default HTTP port, not 8080
    strcpy(_Result->path, "/");

    // Remove "http://" or "https://" prefix
    const char *start = _URL;
    if (strncmp(_URL, "http://", 7) == 0)
    {
        start += 7;
    }
    else if (strncmp(_URL, "https://", 8) == 0)
    {
        start += 8;
        _Result->port = 443; // Default HTTPS port
    }

    // Copy the host part (until '/' or ':')
    const char *path_start = strchr(start, '/');
    const char *port_start = strchr(start, ':');
    
    // Determine where host ends
    const char *host_end = start + strlen(start); // Default to end of string
    if (path_start && (!port_start || path_start < port_start)) {
        host_end = path_start;
    } else if (port_start) {
        host_end = port_start;
    }
    
    // Copy host
    size_t host_len = host_end - start;
    if (host_len >= sizeof(_Result->host)) {
        return -1; // Host too long
    }
    strncpy(_Result->host, start, host_len);
    _Result->host[host_len] = '\0';
    
    // Parse port if present
    if (port_start && (!path_start || port_start < path_start)) {
        _Result->port = atoi(port_start + 1);
    }
    
    // Parse path if present
    if (path_start) {
        strncpy(_Result->path, path_start, sizeof(_Result->path) - 1);
        _Result->path[sizeof(_Result->path) - 1] = '\0';
    }

    return 0;
}

int HTTPClient_Get(const char *_URL)
{
    ParsedURL parsed;

    if (HTTPClient_ParseURL(_URL, &parsed) != 0)
    {
        fprintf(stderr, "Failed to parse URL\r\n");
        return -1;
    }
    
    printf("Connecting to %s:%d%s\n", parsed.host, parsed.port, parsed.path);

    // Build HTTP GET request string
    char request[1024];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             parsed.path, parsed.host);

    uint8_t response[4096];

    TCPClient client;
    TCPClient_Initiate(&client, -1);  // -1 for new client

    // Convert port to string
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", parsed.port);

    if (TCPClient_Connect(&client, parsed.host, port_str) != 0) {
        fprintf(stderr, "Failed to connect\n");
        return -1;
    }

    if (TCPClient_Write(&client, (uint8_t *)request, strlen(request)) < 0) {
        fprintf(stderr, "Failed to send request\n");
        TCPClient_Disconnect(&client);
        TCPClient_Dispose(&client);
        return -1;
    }

    int bytes_read = TCPClient_Read(&client, response, sizeof(response) - 1);
    if (bytes_read > 0) {
        response[bytes_read] = '\0';  // Null terminate
        printf("Response:\n%s\n", response);
    }

    TCPClient_Disconnect(&client);
    TCPClient_Dispose(&client);
    
    return 0;
}