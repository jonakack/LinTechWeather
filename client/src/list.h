#ifndef LIST_H
#define LIST_H

#define FOUND 0
#define NOT_FOUND 1

#include <time.h>

typedef struct cityList
{
    char *name;
    double latitude;
    double longitude;
    
    char *filename;

    struct cityList *next;
    struct cityList *prev;
} cityList;



void List_InitCities();
void List_CreateCity(char *_Name, char *_Lat, char *_Lon);
void List_RemoveCityFromList(cityList **_Head, char* _Name);
void List_ShowCities();
int List_AddCityToList(cityList *_City);
cityList *List_SearchList(char *_CityName);
cityList *List_InitCity();

#endif // LIST_H