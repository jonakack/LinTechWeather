#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[BUF_SIZE] = {0};

    // 1. Skapa socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Binda socket till port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Lyssna på inkommande anslutningar
    listen(server_fd, 3);
    printf("Server listening on port %d...\n", PORT);
    socklen_t addrlen = sizeof(address);
    client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (client_fd < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // 4. Läs data från klienten
    read(client_fd, buffer, BUF_SIZE);
    printf("Received: %s\n", buffer);

    // 5. Skicka tillbaka ett svar
    char *response = "Hello from server!";
    send(client_fd, response, strlen(response), 0);
    close(client_fd);
    close(server_fd);
    
    return 0;
}