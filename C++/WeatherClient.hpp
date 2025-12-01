#pragma once

#include <string>

class WeatherClient 
{
    public:
        explicit WeatherClient(const std::string &serverHost, int serverPort = 8080);
        
        // Hämta väderdata (cachar alltid automatiskt)
        std::string fetchWeather(const std::string &location);
        
        // Tvinga uppdatering från server (ignorerar cache)
        std::string fetchWeatherFresh(const std::string &location);
        

    private:
        std::string host;
        int port;
        
        // Generera cache-filnamn från plats
        std::string getCachePath(const std::string &location) const;
        
        // Intern funktion som hämtar data
        std::string doFetch(const std::string &location, bool useCache);
};