#ifndef CACHE_H
#define CACHE_H
#include "WeatherData.h"

#define ERROR 1
#define OK 0
#define UP_TO_DATE 0
#define OUT_OF_DATE -1
#define DOES_NOT_EXIST -2

// Check if geo data exists in cache and is up to date
int Cache_CheckExistingGeoData(GeoData *_Data);

// Check if weather data exists in cache and is up to date
int Cache_CheckExistingWeatherData(WeatherData *_Data);

// Save geo data to cache (takes raw JSON string)
int Cache_SaveGeoDataJson(const char *_City, const char *_JsonData);

// Save weather data to cache (takes raw JSON string)
int Cache_SaveWeatherDataJson(const char *_Latitude, const char *_Longitude, const char *_JsonData);

// Helper functions
int Cache_CheckDataAge(char *_Filename);
int Cache_CheckDataTime(char *_Filename);

#endif // CACHE_H