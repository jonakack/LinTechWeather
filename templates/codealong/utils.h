#include <time.h>
#include <stdint.h>

uint64_t SystemMonotonicMS()
{
    long ms;
    time_t s;

    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    s = spec.tv_sec;
    ms = (spec.tv_nsec / 1000000);


}