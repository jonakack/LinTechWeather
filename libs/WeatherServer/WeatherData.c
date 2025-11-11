#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "WeatherData.h"
#include "utils.h"

GeoData* WeatherData_ParseGeoRequest(const char* _Url) {
    if (strncmp(_Url, "/api/v1/geo", 11) != 0) {
        return NULL;
    }
    
    // Allocate GeoData structure
    GeoData* data = (GeoData*)calloc(1, sizeof(GeoData));
    if (!data) return NULL;
    
    // Get city parameter
    data->city = get_query_param(_Url, "city");
    if (!data->city) {
        WeatherData_FreeGeoData(data);
        return NULL;
    }
    
    // For now, hardcode some example values
    if (strcmp(data->city, "Stockholm") == 0) {
        data->country = strdup("SE");
        data->lat = 59.3293;
        data->lon = 18.0686;
    } else {
        // Default values for unknown cities
        data->country = strdup("Unknown");
        data->lat = 0.0;
        data->lon = 0.0;
    }
    
    return data;
}

WeatherData* WeatherData_ParseWeatherRequest(const char* _Url) {
    // Check if URL starts with weather API endpoint
    if (strncmp(_Url, "/api/v1/weather", 14) != 0) {
        return NULL;
    }
    
    // Get lat and lon parameters
    char* lat_str = get_query_param(_Url, "lat");
    char* lon_str = get_query_param(_Url, "lon");
    
    if (!lat_str || !lon_str) {
        if (lat_str) free(lat_str);
        if (lon_str) free(lon_str);
        return NULL;
    }
    
    // Convert to doubles
    double lat = str_to_double(lat_str);
    double lon = str_to_double(lon_str);
    
    free(lat_str);
    free(lon_str);
    
    // Allocate WeatherData structure
    WeatherData* data = (WeatherData*)calloc(1, sizeof(WeatherData));
    if (!data) return NULL;
    
    // For now, return hardcoded weather data for Stockholm coordinates
    if (lat == 59.3293 && lon == 18.0686) {
        data->tempC = 7.0;
        data->description = strdup("0");
        data->updatedAt = strdup("2025-11-02T09:00:00Z");
    } else {
        // Default values for unknown locations
        data->tempC = 0.0;
        data->description = strdup("Unknown");
        data->updatedAt = strdup("2025-11-02T09:00:00Z");
    }
    
    return data;
}

char* WeatherData_GeoToJson(const GeoData* _Data) {
    if (!_Data) return NULL;
    
    // Allocate buffer for JSON string
    char* json = (char*)malloc(256);
    if (!json) return NULL;
    
    // Format JSON string
    snprintf(json, 256,
             "{ \"city\":\"%s\",\"country\":\"%s\",\"lat\":%.4f,\"lon\":%.4f }",
             _Data->city,
             _Data->country,
             _Data->lat,
             _Data->lon);
    
    return json;
}

char* WeatherData_WeatherToJson(const WeatherData* _Data) {
    if (!_Data) return NULL;
    
    // Allocate buffer for JSON string
    char* json = (char*)malloc(256);
    if (!json) return NULL;
    
    // Format JSON string
    snprintf(json, 256,
             "{ \"tempC\":%.1f,\"description\":\"%s\",\"updatedAt\":\"%s\" }",
             _Data->tempC,
             _Data->description,
             _Data->updatedAt);
    
    return json;
}

void WeatherData_FreeGeoData(GeoData* _Data) {
    if (!_Data) return;
    
    if (_Data->city) free(_Data->city);
    if (_Data->country) free(_Data->country);
    free(_Data);
}

void WeatherData_FreeWeatherData(WeatherData* _Data) {
    if (!_Data) return;
    
    if (_Data->description) free(_Data->description);
    if (_Data->updatedAt) free(_Data->updatedAt);
    free(_Data);
}