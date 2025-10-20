#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// main.c → http.c → tcp.c → Network

#define PORT 8080

int main()
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // 1. Skapa socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation error");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 2. Konvertera IP-adress
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        return -1;
    }

    // 3. Anslut till servern
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }

    // 4. Skicka meddelande
    char *hello = "Hello from client!";
    send(sock, hello, strlen(hello), 0);

    // 5. Läs svar
    read(sock, buffer, 1024);
    printf("Server: %s\n", buffer);
    close(sock);

    return 0;
}