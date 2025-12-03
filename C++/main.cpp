#include <iostream>
#include "HTTPClient.hpp"
#include "WeatherClient.hpp"

int main()
{
    //HttpClient client("kontoret.onvo.se", 10380);

    WeatherClient wclient("api.open-meteo.com", 80);
    std::string response = wclient.fetchWeather("latitude=64.7507&longitude=20.9528&current_weather=true");
    
    //std::string response = client.get("/api/v1/weather?lat=59.3293&lon=18.0686");

    std::cout << response << std::endl;
    return 0;
}