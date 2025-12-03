#pragma once

#include <string>

class HttpClient
{
    private:
        int socketFd;
        std::string host;
        int port;

    public:
        // Konstruktor deklareras
        HttpClient(const std::string &host, int port = 80);

        // Destruktor - stänger socket automatiskt 
        ~HttpClient();

        // Publik metod för GET-request
        std::string get(const std::string &path);
        std::string extractBody(const std::string &response);
};