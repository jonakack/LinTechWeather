#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include "cache.h"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

static int Cache_MkdirRecursive(const char* _Path)
{
    char tmp[512];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", _Path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            if (MKDIR(tmp) != 0 && errno != EEXIST)
            {
                return ERROR;
            }
            *p = '/';
        }
    }

    if (MKDIR(tmp) != 0 && errno != EEXIST)
    {
        return ERROR;
    }

    return OK;
}

void Cache_BuildPath(char* _Buffer, size_t _BufferSize, const char* _CacheKey, const CacheConfig* _Config)
{
    if (_Buffer == NULL || _CacheKey == NULL || _Config == NULL)
    {
        return;
    }
    snprintf(_Buffer, _BufferSize, "%s/%s.json", _Config->base_path, _CacheKey);
}

int Cache_Check(const char* _CacheKey, const CacheConfig* _Config)
{
    if (_CacheKey == NULL || _Config == NULL)
    {
        printf("Invalid parameters for Cache_Check\n");
        return ERROR;
    }

    char full_path[512];
    Cache_BuildPath(full_path, sizeof(full_path), _CacheKey, _Config);

    struct stat fileStatus;

    if (stat(full_path, &fileStatus) != OK)
    {
        if (errno == ENOENT)
        {
            printf("Cache file does not exist: %s\n", full_path);
            return DOES_NOT_EXIST;
        }
        return ERROR;
    }

    time_t mod_time = fileStatus.st_mtime;
    time_t current_time = time(NULL);

    if ((current_time - mod_time) > _Config->ttl_seconds)
    {
        printf("Cache file is outdated: %s\n", full_path);
        return OUT_OF_DATE;
    }

    printf("Cache file is up to date: %s\n", full_path);
    return UP_TO_DATE;
}

int Cache_Save(const char* _CacheKey, const char* _Data, const CacheConfig* _Config)
{
    if (_CacheKey == NULL || _Data == NULL || _Config == NULL)
    {
        printf("Error: Invalid parameters for Cache_Save\n");
        return ERROR;
    }

    FILE *fptr = NULL;
    char full_path[512];

    // Dubbelkolla att katalogen finns
    if (Cache_MkdirRecursive(_Config->base_path) != OK) 
    {
        perror("Failed to create cache directory");
        return ERROR;
    }
    // Bygg fullständig sökväg för cache-filen
    Cache_BuildPath(full_path, sizeof(full_path), _CacheKey, _Config);

    if ((fptr = fopen(full_path, "w")) == NULL)
    {
        perror("Failed to open file for writing");
        return ERROR;
    }

    size_t data_len = strlen(_Data);
    size_t written = fwrite(_Data, 1, data_len, fptr);
    if (written != data_len)
    {
        perror("Failed to write complete data to file");
        fclose(fptr);
        return ERROR;
    }

    if (fclose(fptr) != OK)
    {
        perror("Failed to close file");
        return ERROR;
    }

    printf("Successfully saved cache: %s\n", full_path);
    return OK;
}

char* Cache_Load(const char* _CacheKey, const CacheConfig* _Config)
{
    if (_CacheKey == NULL || _Config == NULL)
    {
        printf("Invalid parameters for Cache_Load\n");
        return NULL;
    }

    char full_path[512];
    Cache_BuildPath(full_path, sizeof(full_path), _CacheKey, _Config);

    FILE* fptr = fopen(full_path, "r");
    if (fptr == NULL)
    {
        printf("Failed to open cache file: %s\n", full_path);
        return NULL;
    }

    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    char* data = malloc(file_size + 1);
    if (data == NULL)
    {
        printf("Failed to allocate memory for cache data\n");
        fclose(fptr);
        return NULL;
    }

    size_t read_size = fread(data, 1, file_size, fptr);
    data[file_size] = '\0';
    fclose(fptr);

    if (read_size != file_size)
    {
        printf("Failed to read complete cache file\n");
        free(data);
        return NULL;
    }

    printf("Successfully loaded cache: %s\n", full_path);
    return data;
}