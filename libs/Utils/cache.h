#ifndef CACHE_H
#define CACHE_H
#include "WeatherData.h"

#define ERROR 1
#define OK 0
#define UP_TO_DATE 0
#define OUT_OF_DATE -1
#define DOES_NOT_EXIST -2
#define IS_WEATHERDATA -5
#define IS_GEODATA -6

// Checks if file exists, -6 for GeoData, -5 for WeatherData
int Cache_CheckExisting(AllData *_City, int DataType);
int Cache_SaveData(AllData *_Data, char *_Type);
char* Cache_ReadData(AllData *_Data, int DataType);


int Cache_CheckDataAge(char *_Filename);
int Cache_CheckDataTime(char *_Filename);

#endif // CACHE_H