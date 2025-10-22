#include "http_client.h"

//--------------------Internal functions-----------------------

void http_client_work(void* _Context, uint64_t _MonTime);
void http_client_dispose(http_client** _ClientPtr);

//-------------------------------------------------------------

int http_client_init(const char* _URL, http_client** _ClientPtr)
{
    if(_URL == NULL || _ClientPtr ==  NULL)
        return -1;

    if(strlen(_URL) > HTTP_CLIENT_MAX_URL_LENGTH)
        return -2;

    http_client* _Client = (http_client*)malloc(sizeof(http_client));
    if(_Client == NULL)
        return -3;

    _Client->state = http_client_state_init;
    _Client->task = smw_createTask(_Client, http_client_work);
    _Client->timer = 0;

    strcpy(_Client->url, _URL);

    *(_ClientPtr) = _Client;

    return 0;
}

int http_client_get(const char* _URL, uint64_t _Timeout)
{
    http_client* client = NULL;
    if(http_client_init(_URL, &client) != 0)
        return -1;

    client->timeout = _Timeout;

    return 0;
}

void http_client_work(void* _Context, uint64_t _MonTime)
{
    http_client* _Client = (http_client*)_Context;

    if(_Client->timer == 0)
    {
        _Client->timer = _MonTime;
    } 
    else if(_MonTime >= _Client->timer + _Client->timeout)
    {
        http_client_dispose(&_Client);
        return;
    }

    printf("%i > %s\r\n", _Client->state, _Client->url);

    switch(_Client->state)
    {
        case http_client_state_init:
        {
            _Client->state = http_client_state_connect;
        } break;

        case http_client_state_connect:
        {
            // _Client->connect() <- code example
            _Client->state = http_client_state_connecting;
        } break;

        case http_client_state_connecting:
        {
            // if(_Client->isConnected) <- code example
            _Client->state = http_client_state_writing;
        } break;
        
        case http_client_state_writing:
        {
            _Client->state = http_client_state_reading;
        } break;
        
        case http_client_state_reading:
        {
            _Client->state = http_client_state_callback;
        } break;
        
        case http_client_state_callback:
        {
            _Client->state = http_client_state_dispose;
        }

        case http_client_state_dispose:
        {
            http_client_dispose(&_Client);
        } break;
        
    }
}

void http_client_dispose(http_client** _ClientPtr)
{
    if(_ClientPtr == NULL || *(_ClientPtr) == NULL)
        return;

    http_client* _Client = *(_ClientPtr);

    if(_Client->task != NULL)
        smw_destroyTask(_Client->task);
    
    free(_Client);

    *(_ClientPtr) = NULL;
}