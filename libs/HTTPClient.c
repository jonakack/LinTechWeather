#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "TCPClient.h"


void HTTPClient_Get(const char* _Host, const char* _Path)
{
    TCPClient client;
    TCPClient_Initiate(&client);

    TCPClient_Connect(&client, _Host, "8080")

    TCPClient_Write();
    TCPClient_Read();

    TCPClient_Disconnect();
}
