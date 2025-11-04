#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdint.h>
#include <stddef.h>

uint64_t SystemMonotonicMS(void);

char* get_query_param(const char* url, const char* param);
double str_to_double(const char* str);

#endif