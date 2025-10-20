#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "tcp.h"

int tcp_connect(const char* host, int port)
{
    struct sockaddr_in address;
    
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        return -1;
    }
    
    // Set up address structure
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    
    // Convert IP address
    if (inet_pton(AF_INET, host, &address.sin_addr) <= 0) {
        perror("invalid address");
        close(sock);
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("connect failed");
        close(sock);
        return -1;
    }
    
    return sock;
}

int tcp_listen(int port)
{
    struct sockaddr_in address;
    int opt = 1;
    
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return -1;
    }
    
    // Set socket options (reuse address)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return -1;
    }
    
    // Set up address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    // Bind socket to port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }
    
    // Start listening
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        return -1;
    }
    
    return server_fd;
}

int tcp_accept(int server_fd)
{
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    
    int client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (client_fd < 0) {
        perror("accept failed");
        return -1;
    }
    
    return client_fd;
}

ssize_t tcp_send(int fd, const char* data, size_t len)
{
    size_t total_sent = 0;
    
    while (total_sent < len) {
        ssize_t sent = send(fd, data + total_sent, len - total_sent, 0);
        if (sent < 0) {
            perror("send failed");
            return -1;
        }
        total_sent += sent;
    }
    
    return total_sent;
}

ssize_t tcp_receive(int fd, char* buffer, size_t len)
{
    ssize_t received = recv(fd, buffer, len, 0);
    if (received < 0) {
        perror("recv failed");
        return -1;
    }
    
    return received;
}