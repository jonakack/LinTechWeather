#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

const char *stringList = "Stockholm:59.3293:18.0686\n"
                         "Göteborg:57.7089:11.9746\n"
                         "Malmö:55.6050:13.0038\n"
                         "Uppsala:59.8586:17.6389\n"
                         "Västerås:59.6099:16.5448\n"
                         "Örebro:59.2741:15.2066\n"
                         "Linköping:58.4109:15.6216\n"
                         "Helsingborg:56.0465:12.6945\n"
                         "Jönköping:57.7815:14.1562\n"
                         "Norrköping:58.5877:16.1924\n"
                         "Lund:55.7047:13.1910\n"
                         "Gävle:60.6749:17.1413\n"
                         "Sundsvall:62.3908:17.3069\n"
                         "Umeå:63.8258:20.2630\n"
                         "Luleå:65.5848:22.1567\n"
                         "Kiruna:67.8558:20.2253\n"
                         "Skellefteå:64.7506:20.9528\n";

cityList *head;

// Takes stringList, breaks it apart and puts cities in linked list
void List_InitCities()
{
    char *list_copy = strdup(stringList);
    if (list_copy == NULL)
    {
        printf("Failed to allocate memory for list copy\n");
        return;
    }

    char *ptr = list_copy;

    char *name = NULL;
    char *lat_str = NULL;
    char *lon_str = NULL;

    do
    {
        if (name == NULL)
        {
            name = ptr;
        }
        else if (lat_str == NULL)
        {
            if (*(ptr) == ':')
            {
                lat_str = ptr + 1;
                *(ptr) = '\0';
            }
        }
        else if (lon_str == NULL)
        {
            if (*(ptr) == ':')
            {
                lon_str = ptr + 1;
                *(ptr) = '\0';
            }
        }
        else
        {
            if (*(ptr) == '\n')
            {
                *(ptr) = '\0';

                // printf("City: <%s>, Latitude: <%s>, Longitude: <%s>\n", name, lat_str, lon_str);

                List_CreateCity(name, lat_str, lon_str);

                name = NULL;
                lat_str = NULL;
                lon_str = NULL;
            }
        }

        ptr++;

    } while (*(ptr) != '\0');

    free(list_copy);
}
// Creates an empty city struct and returns its address
cityList *List_InitCity()
{
    cityList *List_InitCity = malloc(sizeof(cityList));
    if (List_InitCity == NULL)
    {
        return NULL;
    }
    // Initialize all fields to safe values
    List_InitCity->name = NULL;
    List_InitCity->latitude = 0.0;
    List_InitCity->longitude = 0.0;
    List_InitCity->filename = NULL;
    List_InitCity->next = NULL;
    List_InitCity->prev = NULL;

    return List_InitCity;
}

// Fills empty city struct with name and coordinates
void List_CreateCity(char *_Name, char *_Lat, char *_Lon)
{
    cityList *new_city = List_InitCity();
    if (new_city == NULL)
    {
        printf("Failed to allocate memory for city\n");
        return;
    }

    new_city->name = strdup(_Name);
    new_city->latitude = atof(_Lat);
    new_city->longitude = atof(_Lon);

    // Create filename
    char filename[200];
    sprintf(filename, "data/%s_%.4f_%.4f.json", _Name, atof(_Lat), atof(_Lon));
    new_city->filename = strdup(filename);

    // Check for allocation failures
    if (new_city->name == NULL || new_city->filename == NULL)
    {
        printf("Failed to allocate memory for city strings\n");
        free(new_city->name);
        free(new_city->filename);
        free(new_city);
        return;
    }

    new_city->next = NULL;
    new_city->prev = NULL;

    List_AddCityToList(new_city);
}

int List_AddCityToList(cityList *_City)
{
    if (head == NULL)
    {
        head = _City;
    }
    else
    {
        _City->next = head;
        head->prev = _City;
        head = _City;
    }
    return 0;
}

void List_RemoveCityFromList(cityList **_Head, char *_Name)
{
    cityList *current = *_Head;
    while (current != NULL)
    {
        if (strcmp(current->name, _Name) == 0)
        { /* Break loop if they match */
            if (current->prev != NULL)
            {
                current->prev->next = current->next;
            }
            else
            {
                // Removing head node
                *_Head = current->next;
            }
            if (current->next != NULL)
            {
                current->next->prev = current->prev;
            }
            free(current->name);
            free(current->filename);
            free(current);
            printf("Removed %s\n", _Name);
            break;
        }
        else
            current = current->next;
    }
    if (current == NULL)
    {
        printf("City not found\n");
    }
}

void List_ShowCities()
{
    cityList *current = head;
    while (current != NULL)
    {
        printf("%s\n", current->name);
        current = current->next;
    }
    current = head; /* Resets *current to starting position */
}

cityList *List_SearchList(char *_CityName)
{
    cityList *current = head;
    while (current != NULL)
    {
        if ((strcmp(_CityName, current->name)) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}