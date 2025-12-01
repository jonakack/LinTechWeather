#include <iostream>
#include <stdexcept>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include "HTTPClient.hpp"


HttpClient::HttpClient(const std::string &host, int port)
    : host(host), port(port)
{
    // Skapa socket
    socketFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
        throw std::runtime_error("Failed to create socket");

    // Förbered adress struktur
    sockaddr_in addr{}; 
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    // Convert host to IP-address
    struct hostent* hostEntry = gethostbyname(host.c_str());
    if (hostEntry == nullptr) 
    {
        ::close(socketFd);
        throw std::runtime_error("Failed to resolve hostname: " + host);
    }

    std::memcpy(&addr.sin_addr, hostEntry->h_addr, hostEntry->h_length);

    // Anslut   
    if (::connect(socketFd, (sockaddr *)&addr, sizeof(addr)) < 0) 
    {
        ::close(socketFd);
        throw std::runtime_error("Failed to connect" + host);
    } std::cout << "Connected to " + host << '\n';

}

HttpClient::~HttpClient()
{
    if (socketFd >= 0)
    {
        ::close(socketFd);
    }
}

std::string HttpClient::get(const std::string &path) {
    // Bygg HTTP-request
    std::string request =
        "GET " + path + " HTTP/1.1\r\n"
        "Host: " + host + "\r\n"
        "Connection: close\r\n"
        "\r\n";

    // Skicka request
    ssize_t sent = ::send(socketFd, request.c_str(), request.size(), 0);
    if (sent < 0) {
        throw std::runtime_error("Failed to send request");
    }

    // Ta emot svar
    char buffer[4096];
    std::string response;

    ssize_t bytesRead;  
    while ((bytesRead = ::recv(socketFd, buffer, sizeof(buffer), 0)) > 0) {
        response.append(buffer, bytesRead);
    }

    if (bytesRead < 0) {
        throw std::runtime_error("Failed to receive response");
    }

    return extractBody(response);
}

// Ignore headers and return body
std::string HttpClient::extractBody(const std::string &response) {
    // Hitta första {
    auto start = response.find("{");
    if (start == std::string::npos) {
        return "";
    }

    // Hitta matchande }
    auto end = response.rfind("}}" + 1);
    if (end == std::string::npos || end <= start) {
        return "";
    }

    // substr(start, length)
    return response.substr(start, end - start + 1);
}
