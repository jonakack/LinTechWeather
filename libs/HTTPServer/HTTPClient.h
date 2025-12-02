#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include "WeatherData.h"

char *HTTPClient_Get(char *_Host, char *_Port, char *_Request);

int HTTPClient_GetGeoData(AllData* _Data);

int HTTPClient_GetWeatherData(AllData *_Data);

#endif // _HTTPCLIENT_H_