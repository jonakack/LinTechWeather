#ifndef __WeatherRequestHandler_h_
#define __WeatherRequestHandler_h_

// Handle geo request (/api/v1/geo?city=...)
// Returns allocated JSON string on success, NULL on error.
// Caller must free returned string.
char* WeatherRequestHandler_HandleGeoRequest(const char* _Url);

// Handle weather request (/api/v1/weather?lat=...&lon=...)
// Returns allocated JSON string on success, NULL on error.
// Caller must free returned string.
char* WeatherRequestHandler_HandleWeatherRequest(const char* _Url);

#endif // __WeatherRequestHandler_h_
