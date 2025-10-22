#include <stdio.h>
#include <stdint.h>
#include "../libs/TCPServer.h"

int main()
{
    TCPServer server;
    
    TCPServer_Initiate(&server, "8080");

    while(1)
    {
        TCPServer_Work(&server);
    }

    TCPServer_Dispose(&server);

    return 0;
}
