#include "WeatherClient.hpp"
#include "Cache.hpp"
#include "HTTPClient.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdio>  

WeatherClient::WeatherClient(const std::string &serverHost, int serverPort)
    : host(serverHost), port(serverPort) {}

std::string WeatherClient::fetchWeather(const std::string &location)
{
    return doFetch(location, true);
}

std::string WeatherClient::fetchWeatherFresh(const std::string &location)
{
    return doFetch(location, false); 
}

std::string WeatherClient::doFetch(const std::string &location, bool useCache)
{
    std::string cachePath = getCachePath(location); 
    Cache cache(cachePath);

    // Använd cache om tillgängligt OCH inte äldre än 15 minuter
    if (useCache && cache.exists() && !cache.isOld())
    {
        std::cout << "Using cached data for " << location << std::endl;
        return cache.load();
    }

    std::cout << "Fetching fresh data for " << location << std::endl;

    // Skapa ny HTTP-anslutning för request
    HttpClient client(host, port); 

    // Bygg API-path
    // TODO: bygg om "location" till rätt url, t.ex. "Stockholm" -> leta i linked list -> /v1/forecast?latitude=64.7507&longitude=20.9528&current_weather=true
    std::string path = "/v1/forecast?" + location;

    // Hämta data från server
    std::string data = client.get(path);

    // Spara ALLTID i cache
    cache.save(data); 
    std::cout << "Cached data for " << location << std::endl;

    return data; 
}

std::string WeatherClient::getCachePath(const std::string &location) const 
{
    return "cache_" + location + ".json";
}
