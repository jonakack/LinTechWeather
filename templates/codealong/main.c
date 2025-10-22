#include <stdio.h>
#include <stdint.h>

#include "smw.h"
#include "http_client.h"
#include "utils.h"

void main_http_callback(int _Event, const char* _Response)
{
    printf("main_http_callback: %s (%s)\r\n", _Event, _Response);
}

int main()
{
    smw_init();

    http_client_get("https://google.se", 1000, main_http_callback);
    http_client_get("https://aftonbladet.se", 1000, main_http_callback);

    uint64_t monTime =  0;
    while(smw_getTaskCount() > 0)
    {
        monTime = SystemMonotonicMS();
        smw_work(monTime);
    }

    smw_dispose();

    return 0;
}