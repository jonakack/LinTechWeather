#include "TCPServer.h"
#include "TCPClient.h"

int TCPServer_Initiate(TCPServer* _Server, const char* _Port)
{
    struct addrinfo hints = {0}, *result = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL, _Port, &hints, &result) != 0)
        return -1;

    int fd = -1;
    for (struct addrinfo *resPtr = result; resPtr; resPtr = resPtr->ai_next)
    {
        fd = socket(resPtr->ai_family, resPtr->ai_socktype, resPtr->ai_protocol);
        if (fd < 0)
            continue;

        int yes = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (bind(fd, resPtr->ai_addr, resPtr->ai_addrlen) == 0)
            break;

        close(fd);
        fd = -1;
    }

    freeaddrinfo(result);
    if (fd < 0)
        return -1;

    if (listen(fd, MAX_CLIENTS) < 0)
    {
        close(fd);
        return -1;
    }

    TCPServer_Nonblocking(fd);

    _Server->listen_fd = fd;
    for (int i = 0; i < MAX_CLIENTS; i++)
        _Server->clients[i].fd = -1;

    printf("Server listening to port %s\r\n", _Port);

    return 0;
}

int TCPServer_Accept(TCPServer* _Server)
{
    // Listens for a connection
    int client_fd = accept(_Server->listen_fd, NULL, NULL);
    if (client_fd < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0; // No new client

        perror("accept");
        return -1;
    }

    TCPServer_Nonblocking(client_fd);

    // Find available spot
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (_Server->clients[i].fd < 0)
        {
            _Server->clients[i].fd = client_fd;
            printf("New client accepted (index %d)\r\n", i);
            return 1;
        }
    }

    // If spots are full
    close(client_fd);
    printf("Max clients, connection denied\r\n");
    return 0;
}

void TCPServer_Work(TCPServer* _Server)
{
    TCPServer_Accept(_Server);

    char buf[512];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        int fd = _Server->clients[i].fd;
        if (fd < 0)
            continue;

        ssize_t n = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
        if (n > 0)
        {
            buf[n] = '\0';
            //printf("recv before send: %s\r\n", buf);
            send(fd, buf, (size_t)n, MSG_NOSIGNAL); // Echo
            printf("recv after send: %s\r\n", buf); // "/n" finns nog redan i "buf" därför två newlines händer jag skickar det här till main alex
        }                                           // Ladda gärna upp på github !! <3 gör det nu :D B) 8==D
        else if (n == 0)
        {
            printf("Client %d disconnected\r\n", i);
            close(fd);
            _Server->clients[i].fd = -1;
        }
    }
}

void TCPServer_Dispose(TCPServer* _Server)
{
    close(_Server->listen_fd);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (_Server->clients[i].fd >= 0)
            close(_Server->clients[i].fd);
    }
}