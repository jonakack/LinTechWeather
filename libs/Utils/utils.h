#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

uint64_t SystemMonotonicMS(void);

char *get_query_param(const char *url, const char *param);
double str_to_double(const char *str);

bool validate_latitude(const char *lat_str);
bool validate_longitude(const char *lon_str);
bool validate_city_name(const char *city);
char *url_decode(const char *src);

#endif