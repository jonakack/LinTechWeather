#ifndef HTTP_H
#define HTTP_H

typedef struct {
    char version[32];
    int status_code;
    char text[32];
} statusline;

typedef struct {
    char contentlength[64];
    char contenttype[64];
} header;

typedef struct {
    statusline status_line;
    header headers[20];
    int header_count;
    char body[1024];
} httprespone;




#endif // HTTP_H