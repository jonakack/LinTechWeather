#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

typedef struct {
    char host[256];
    int port;
    char path[1024];
} ParsedURL;

int HTTPClient_ParseURL(const char* _URL, ParsedURL* _ParsedURL);
int HTTPClient_Get(const char* _URL);

#endif // HTTP_CLIENT_H