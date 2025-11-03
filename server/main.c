#include <stdio.h>
#include <stdint.h>
#include "../libs/TCPServer.h"

// Callback function to handle client connections
int handle_client(int _Client_fd, void* _Context)
{
    printf("New client connected (fd: %d)\n", _Client_fd);
    
    // Handle the client connection here
    // For now, just close it
    close(_Client_fd);
    
    return 0;
}

int main()
{
    TCPServer server;
    
    // Initialize server with callback function
    if (TCPServer_Initiate(&server, "8080", handle_client, NULL) != 0) {
        printf("Failed to initialize server\n");
        return 1;
    }

    printf("Server listening on port 8080...\n");

    // Keep the server running
    // Note: The actual work is done by the task system
    while(1)
    {
        sleep(1);  // Sleep to prevent busy waiting
    }

    TCPServer_Dispose(&server);

    return 0;
}
