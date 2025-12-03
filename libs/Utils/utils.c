#define _POSIX_C_SOURCE 200809L

#include "utils.h"


uint64_t SystemMonotonicMS(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

// Helper function to extract query parameter value.
// Skips the '?' and '<param>='
char *get_query_param(const char *url, const char *param)
{
    char *query = strstr(url, "?");
    if (!query)
        return NULL;

    query++; // Skip the '?'

    char param_prefix[32];
    snprintf(param_prefix, sizeof(param_prefix), "%s=", param);

    char *param_start = strstr(query, param_prefix);
    if (!param_start)
        return NULL;

    param_start += strlen(param_prefix);

    // Find end of parameter (& or end of string)
    char *param_end = strchr(param_start, '&');
    if (!param_end)
        param_end = param_start + strlen(param_start);

    // Allocate and copy the parameter value
    size_t param_len = param_end - param_start;
    char *value = (char *)malloc(param_len + 1);
    if (!value)
        return NULL;

    strncpy(value, param_start, param_len);
    value[param_len] = '\0';

    return value;
}

// Helper function to convert string to double
double str_to_double(const char *str)
{
    if (!str)
        return 0.0;
    return atof(str);
}

// Validera latitude: måste vara mellan -90 och +90
bool validate_latitude(const char *lat_str)
{
    if (!lat_str)
        return false;

    char *endptr;
    double lat = strtod(lat_str, &endptr);

    // Kontrollera att hela strängen parsades
    if (*endptr != '\0')
        return false;

    // Kontrollera räckvidd
    return (lat >= -90.0 && lat <= 90.0);
}

// Validera longitude: måste vara mellan -180 och +180
bool validate_longitude(const char *lon_str)
{
    if (!lon_str)
        return false;

    char *endptr;
    double lon = strtod(lon_str, &endptr);

    // Kontrollera att hela strängen parsades
    if (*endptr != '\0')
        return false;

    // Kontrollera räckvidd
    return (lon >= -180.0 && lon <= 180.0);
}

// Validera city name: max 100 tecken, endast bokstäver, siffror, mellanslag och vissa specialtecken
bool validate_city_name(const char *city)
{
    if (!city)
        return false;

    size_t len = strlen(city);

    // Längd-begränsning
    if (len == 0 || len > 100)
        return false;

    // Karaktär-validering
    for (size_t i = 0; i < len; i++)
    {
        char c = city[i];
        // Tillåt: bokstäver, siffror, mellanslag, bindestreck, punkt, komma
        if (!isalnum(c) && c != ' ' && c != '-' && c != '.' && c != ',' && c != '\'')
        {
            return false;
        }
    }

    return true;
}

// URL-decode function (for percent-encoding)
char *url_decode(const char *src)
{
    if (!src)
        return NULL;

    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    if (!decoded)
        return NULL;

    size_t j = 0;
    for (size_t i = 0; i < src_len; i++)
    {
        if (src[i] == '%' && i + 2 < src_len)
        {
            // Convert hex to character
            int hex_val;
            if (sscanf(src + i + 1, "%2x", &hex_val) == 1)
            {
                decoded[j++] = (char)hex_val;
                i += 2;
            }
            else
            {
                decoded[j++] = src[i];
            }
        }
        else if (src[i] == '+')
        {
            // '+' representerar mellanslag i URL:er
            decoded[j++] = ' ';
        }
        else
        {
            decoded[j++] = src[i];
        }
    }
    decoded[j] = '\0';

    return decoded;
}
// Check if URL matches a specific path
int compare_url(const char* _Url, const char* _Path)
{
    if (_Url == NULL || _Path == NULL) {
        return 0;
    }

    size_t path_len = strlen(_Path);

    // URL must start with path
    if (strncmp(_Url, _Path, path_len) != 0) {
        return 0;
    }

    // After path must be end of string, '?' or other separator
    char next_char = _Url[path_len];
    return (next_char == '\0' || next_char == '?' || next_char == '#');
}