#include "WeatherRequestHandler.h"
#include "WeatherData.h"
#include <stdlib.h>

char* WeatherRequestHandler_HandleGeoRequest(const char* _Url)
{
    if (_Url == NULL) {
        return NULL;
    }

    // Parse geo request from URL
    GeoData* geo_data = WeatherData_ParseGeoRequest(_Url);
    if (geo_data == NULL) {
        return NULL;
    }

    // Convert to JSON
    char* json_response = WeatherData_GeoToJson(geo_data);

    // Free geo data
    WeatherData_FreeGeoData(geo_data);

    return json_response;
}

char* WeatherRequestHandler_HandleWeatherRequest(const char* _Url)
{
    if (_Url == NULL) {
        return NULL;
    }

    // Parse weather request from URL
    WeatherData* weather_data = WeatherData_ParseWeatherRequest(_Url);
    if (weather_data == NULL) {
        return NULL;
    }

    // Convert to JSON
    char* json_response = WeatherData_WeatherToJson(weather_data);

    // Free weather data
    WeatherData_FreeWeatherData(weather_data);

    return json_response;
}
