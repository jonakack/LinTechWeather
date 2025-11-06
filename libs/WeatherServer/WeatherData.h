#ifndef __WeatherData_h_
#define __WeatherData_h_

#include "HTTPServerConnection.h"

// Structure to hold parsed URL parameters for geo endpoint
typedef struct {
    char* city;
    char* country;
    double lat;
    double lon;
} GeoData;

// Structure to hold weather data
typedef struct {
    double tempC;
    char* description;
    char* updatedAt;
} WeatherData;

// Parse the URL and return geo data
GeoData* WeatherData_ParseGeoRequest(const char* url);

// Parse the URL and return weather data
WeatherData* WeatherData_ParseWeatherRequest(const char* url);

// Convert geo data to JSON string
char* WeatherData_GeoToJson(const GeoData* data);

// Convert weather data to JSON string
char* WeatherData_WeatherToJson(const WeatherData* data);

// Free the geo data structure
void WeatherData_FreeGeoData(GeoData* data);

// Free the weather data structure
void WeatherData_FreeWeatherData(WeatherData* data);

#endif // __WeatherData_h_