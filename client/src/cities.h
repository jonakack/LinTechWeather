#include <stdbool.h>

#ifndef CITIES_H
#define CITIES_H

#define YES 0
#define NO -1

int Cities_GetData(const char *_Choice);
int Cities_DisplayData(char *_Filename, char *_CityName);
void Cities_GetChoice(char *_UserChoice);

#endif