#ifndef __WeatherRequestHandler_h_
#define __WeatherRequestHandler_h_

typedef struct {
    char method[16];        // GET, POST, etc.
    char path[256];         // /api/v1/weather
    char query_string[512]; // lat=59.3293&lon=18.0686
    char version[16];       // HTTP/1.1
} HTTPRequest;

// Handle geo request (/api/v1/geo?city=...)
// Returns allocated JSON string on success, NULL on error.
// Caller must free returned string.
char* WeatherRequestHandler_HandleGeoRequest(const char* _Url);

// Handle weather request (/api/v1/weather?lat=...&lon=...)
// Returns allocated JSON string on success, NULL on error.
// Caller must free returned string.
char* WeatherRequestHandler_HandleWeatherRequest(const char* _Url);

#endif // __WeatherRequestHandler_h_
