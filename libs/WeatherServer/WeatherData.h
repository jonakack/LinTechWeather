#ifndef __WeatherData_h_
#define __WeatherData_h_

#include "HTTPServerConnection.h"


typedef struct {

    char* city;
    char* response;

} GeoData;

typedef struct {

    char* latitude;
    char* longitude;
    char* response;

} WeatherData;

// Parse the URL and return geo data
GeoData* WeatherData_ParseGeoRequest(const char* _Url);

// Parse the URL and return weather data
WeatherData* WeatherData_ParseWeatherRequest(const char* _Url);

// Convert geo data to JSON string
char* WeatherData_GeoToJson(const GeoData* _Data);

// Convert weather data to JSON string
char* WeatherData_WeatherToJson(const WeatherData* _Data);

// Free the geo data structure
void WeatherData_FreeGeoData(GeoData* _Data);

// Free the weather data structure
void WeatherData_FreeWeatherData(WeatherData* _Data);

#endif // __WeatherData_h_