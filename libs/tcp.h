#ifndef TCP_H
#define TCP_H

#include <sys/types.h>

// AI generated function suggestions, pure TCP utilities
int tcp_connect(const char* host, int port);
int tcp_listen(int port);
int tcp_accept(int server_fd);
ssize_t tcp_send(int fd, const char* data, size_t len);
ssize_t tcp_receive(int fd, char* buffer, size_t len);

#endif // TCP_H