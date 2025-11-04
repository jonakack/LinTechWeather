#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "utils.h"

uint64_t SystemMonotonicMS(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

// Helper function to extract query parameter value
char* get_query_param(const char* url, const char* param) {
    char* query = strstr(url, "?");
    if (!query) return NULL;
    
    query++; // Skip the '?'
    
    char param_prefix[32];
    snprintf(param_prefix, sizeof(param_prefix), "%s=", param);
    
    char* param_start = strstr(query, param_prefix);
    if (!param_start) return NULL;
    
    param_start += strlen(param_prefix);
    
    // Find end of parameter (& or end of string)
    char* param_end = strchr(param_start, '&');
    if (!param_end) param_end = param_start + strlen(param_start);
    
    // Allocate and copy the parameter value
    size_t param_len = param_end - param_start;
    char* value = (char*)malloc(param_len + 1);
    if (!value) return NULL;
    
    strncpy(value, param_start, param_len);
    value[param_len] = '\0';
    
    return value;
}

// Helper function to convert string to double
double str_to_double(const char* str) {
    if (!str) return 0.0;
    return atof(str);
}