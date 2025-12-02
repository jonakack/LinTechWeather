#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h> // Set compiler path as C:/msys64/ucrt64/bin/gcc.exe on windows
#include "cache.h"
#include "cJSON.h"

// Macro to make mkdir work on Windows and Linux
#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

int Cache_CheckExistingGeoData(GeoData *_Data)
{
    if (_Data == NULL || _Data->city == NULL)
    {
        printf("Invalid GeoData\n");
        return ERROR;
    }

    char folder[100];
    char target_filename[100];
    char full_path[200];

    snprintf(folder, sizeof(folder), "./libs/WeatherServer/cache/geodata");
    sprintf(target_filename, "%s.json", _Data->city);
    sprintf(full_path, "%s/%s", folder, target_filename);

    DIR *dir = opendir(folder);
    if (dir == NULL)
    {
        printf("Could not open folder %s\n", folder);
        return DOES_NOT_EXIST;
    }

    // Loop through all files in the directory
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, target_filename) == 0)
        {
            closedir(dir);
            int cache_status = Cache_CheckDataAge(full_path);
            if (cache_status == OUT_OF_DATE)
            {
                printf("File is outdated\n");
                return OUT_OF_DATE;
            }
            else if (cache_status == UP_TO_DATE)
            {
                printf("File is up to date\n");
                return UP_TO_DATE;
            }
        }
    }
    closedir(dir);
    printf("File does not exist\n");
    return DOES_NOT_EXIST;
}

int Cache_CheckExistingWeatherData(WeatherData *_Data)
{
    if (_Data == NULL || _Data->latitude == NULL || _Data->longitude == NULL)
    {
        printf("Invalid WeatherData\n");
        return ERROR;
    }

    char folder[100];
    char target_filename[100];
    char full_path[200];

    snprintf(folder, sizeof(folder), "./libs/WeatherServer/cache/weatherdata");
    sprintf(target_filename, "%s_%s.json", _Data->latitude, _Data->longitude);
    sprintf(full_path, "%s/%s", folder, target_filename);

    DIR *dir = opendir(folder);
    if (dir == NULL)
    {
        printf("Could not open folder %s\n", folder);
        return DOES_NOT_EXIST;
    }

    // Loop through all files in the directory
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, target_filename) == 0)
        {
            closedir(dir);
            int cache_status = Cache_CheckDataAge(full_path);
            if (cache_status == OUT_OF_DATE)
            {
                printf("File is outdated\n");
                return OUT_OF_DATE;
            }
            else if (cache_status == UP_TO_DATE)
            {
                printf("File is up to date\n");
                int dataTimeResult = UP_TO_DATE;
                if ((dataTimeResult = Cache_CheckDataTime(full_path)) == OUT_OF_DATE)
                {
                    return OUT_OF_DATE;
                }
                return UP_TO_DATE;
            }
        }
    }
    closedir(dir);
    printf("File does not exist\n");
    return DOES_NOT_EXIST;
}

int Cache_SaveGeoDataJson(const char *_City, const char *_JsonData)
{
    if (_City == NULL || _JsonData == NULL)
    {
        printf("Error: Invalid parameters for Cache_SaveGeoDataJson\n");
        return ERROR;
    }

    FILE *fptr = NULL;
    char filename[256];
    int result = -3;

    // Create cache directory structure
    if (MKDIR("./libs/WeatherServer/cache") != 0 && errno != EEXIST)
    {
        perror("Failed to create cache directory");
        result = ERROR;
        goto cleanup;
    }

    // Create geodata directory
    if (MKDIR("./libs/WeatherServer/cache/geodata") != 0 && errno != EEXIST)
    {
        perror("Failed to create geodata directory");
        result = ERROR;
        goto cleanup;
    }

    // Create filename
    sprintf(filename, "./libs/WeatherServer/cache/geodata/%s.json", _City);

    // Write to file
    if ((fptr = fopen(filename, "w")) == NULL)
    {
        perror("Failed to open file for geo data");
        result = ERROR;
        goto cleanup;
    }

    size_t json_len = strlen(_JsonData);
    size_t written = fwrite(_JsonData, 1, json_len, fptr);
    if (written != json_len)
    {
        perror("Failed to write complete JSON to file");
        result = ERROR;
        goto cleanup;
    }

    if (fclose(fptr) != OK)
    {
        perror("Failed to close file");
        result = ERROR;
        goto cleanup;
    }
    fptr = NULL;
    result = OK; // Success
    printf("Successfully saved geo data cache for %s\n", _City);

cleanup:
    if (fptr != NULL)
    {
        fclose(fptr);
    }
    return result;
}

int Cache_SaveWeatherDataJson(const char *_Latitude, const char *_Longitude, const char *_JsonData)
{
    if (_Latitude == NULL || _Longitude == NULL || _JsonData == NULL)
    {
        printf("Error: Invalid parameters for Cache_SaveWeatherDataJson\n");
        return ERROR;
    }

    FILE *fptr = NULL;
    char filename[256];
    int result = -3;

    // Create cache directory structure
    if (MKDIR("./libs/WeatherServer/cache") != 0 && errno != EEXIST)
    {
        perror("Failed to create cache directory");
        result = ERROR;
        goto cleanup;
    }

    // Create weatherdata directory
    if (MKDIR("./libs/WeatherServer/cache/weatherdata") != 0 && errno != EEXIST)
    {
        perror("Failed to create weatherdata directory");
        result = ERROR;
        goto cleanup;
    }

    // Create filename
    sprintf(filename, "./libs/WeatherServer/cache/weatherdata/%s_%s.json", _Latitude, _Longitude);

    // Write to file
    if ((fptr = fopen(filename, "w")) == NULL)
    {
        perror("Failed to open file for weather data");
        result = ERROR;
        goto cleanup;
    }

    size_t json_len = strlen(_JsonData);
    size_t written = fwrite(_JsonData, 1, json_len, fptr);
    if (written != json_len)
    {
        perror("Failed to write complete JSON to file");
        result = ERROR;
        goto cleanup;
    }

    if (fclose(fptr) != OK)
    {
        perror("Failed to close file");
        result = ERROR;
        goto cleanup;
    }
    fptr = NULL;
    result = OK; // Success
    printf("Successfully saved weather data cache for %s, %s\n", _Latitude, _Longitude);

cleanup:
    if (fptr != NULL)
    {
        fclose(fptr);
    }
    return result;
}

int Cache_CheckDataAge(char *_Filename)
{
    struct stat fileStatus;

    // Check if file exists
    if (stat(_Filename, &fileStatus) != OK)
    {
        if (errno == ENOENT)
        {
            return DOES_NOT_EXIST;
        }
        else
            return ERROR;
    }
    printf("File exists\n");

    time_t mod_time = fileStatus.st_mtime;
    time_t current_time = time(NULL);

    if ((current_time - mod_time) > 900) // Checks if file is older than 15 minutes
    {
        return OUT_OF_DATE;
    }
    return UP_TO_DATE;
}

/*  This function checks the API time inside of a JSON file. Needs full path.
    Returns -1 if outdated, 0 if up to date */
int Cache_CheckDataTime(char *_Filename)
{
    FILE *fptr;

    // Open file
    fptr = fopen(_Filename, "r");
    if (fptr == NULL)
    {
        printf("Failed to open file\n");
        return ERROR;
    }

    // Copy content in file
    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    char *content = malloc(file_size + 1);
    if (content == NULL)
    {
        printf("Failed to allocate memory\n");
        return ERROR;
    }

    fread(content, 1, file_size, fptr);
    content[file_size] = '\0';

    fclose(fptr);

    // Find "time":	e.g "2025-10-07T07:00"
    cJSON *json = cJSON_Parse(content);
    if (json == NULL)
    {
        printf("Failed to parse cJSON\n");
        return ERROR;
    }

    cJSON *current_weather = cJSON_GetObjectItem(json, "current_weather");
    if (current_weather == NULL)
    {
        printf("Failed to get current_weather\n");
        cJSON_Delete(json);
        return ERROR;
    }

    cJSON *json_time = cJSON_GetObjectItem(current_weather, "time");
    if (json_time == NULL || !cJSON_IsString(json_time))
    {
        printf("Failed to get time or time is not a string\n");
        cJSON_Delete(json);
        free(content);
        return ERROR;
    }

    char *time_jsonString = json_time->valuestring;
    printf("File's API time: %s\n", time_jsonString);
    // time_jsonString now contains time in the file in YYYY-MM-DDTHH:MM format

    char buffer[20];
    time_t current_time = time(NULL);
    struct tm *gmt = gmtime(&current_time);
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M", gmt);
    printf("Current time: %s\n", buffer);
    // Buffer now contains current GMT time in YYYY-MM-DDTHH:MM format

    // Compare with current time and date
    struct tm file;
    struct tm current;

    sscanf(time_jsonString, "%d-%d-%dT%d:%d",
           &file.tm_year,
           &file.tm_mon,
           &file.tm_mday,
           &file.tm_hour,
           &file.tm_min);

    sscanf(buffer, "%d-%d-%dT%d:%d",
           &current.tm_year,
           &current.tm_mon,
           &current.tm_mday,
           &current.tm_hour,
           &current.tm_min);

    file.tm_year -= 1900;
    file.tm_mon -= 1;
    file.tm_sec = 0;
    file.tm_isdst = -1;

    current.tm_year -= 1900;
    current.tm_mon -= 1;
    current.tm_sec = 0;
    current.tm_isdst = -1;

    // This will make intervals 0, 1, 2 or 3
    int file_interval = file.tm_min / 15;
    int current_interval = current.tm_min / 15;

    if (file.tm_mday != current.tm_mday ||
        file.tm_hour != current.tm_hour ||
        file_interval != current_interval)
    {
        free(content);
        cJSON_Delete(json);
        printf("API is outdated\n");
        return OUT_OF_DATE; // If interval in file is older than current, a new version is available online
    }

    free(content);
    cJSON_Delete(json);
    printf("API is up to date\n");
    return UP_TO_DATE; // Do nothing if they're the same interval
}