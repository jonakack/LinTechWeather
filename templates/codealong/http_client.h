#ifndef _http_client_h_
#define _http_client_h_

#include <stdint.h>
#include "smw.h"

#ifndef HTTP_CLIENT_MAX_URL_LENGTH
    #define HTTP_CLIENT_MAX_URL_LENGTH 1024
#endif

typedef enum
{
    http_client_state_init = 0,
    http_client_state_connect = 1,
    http_client_state_connecting = 2,
    http_client_state_writing = 3,
    http_client_state_reading = 4,
    http_client_state_callback = 5,
    http_client_state_dispose = 6

} http_client_state;

typedef struct
{
    http_client_state state;
    smw_task* task;
    char* url[HTTP_CLIENT_MAX_URL_LENGTH + 1];
    uint64_t timeout;
    uint64_t timer;

    int isConnected;
    
} http_client;

int http_client_get(const char* _URL, uint64_t _Timeout);

#endif // _http_client_h_