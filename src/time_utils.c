#include "../include/time_utils.h"

double get_round_time(struct timespec start)
{
    struct timespec end;
    struct timespec diff;

    clock_gettime(CLOCK_MONOTONIC, &end);
    return (float)(end.tv_sec - start.tv_sec) * 1000 +
           (float)(end.tv_nsec - start.tv_nsec) / 1000000;
}

double get_timestamp()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec +
           time.tv_nsec / 1000000000;
}

short is_time_to_send(ping_config_t args, double next_ts)
{
    if (args.flags & PING_FLOOD)
        return get_timestamp() + args.interval > next_ts - 1;
    return get_timestamp() > next_ts;
}

void update_next_date(struct timeval *next, double interval)
{
    next->tv_usec += interval * 1e6;
    if (next->tv_usec >= 1e6)
    {
        next->tv_usec -= 1e6;
        next->tv_sec++;
    }
}

int time_cmp(struct timeval *next)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return timercmp(&now, next, >=);
}

struct timeval get_timeout(double interval)
{
    struct timeval timeout;
    gettimeofday(&timeout, NULL);
    timeout.tv_sec += (int)interval;
    timeout.tv_usec += (int)(interval * 1e6);
    if (timeout.tv_usec >= 1e6)
    {
        timeout.tv_usec %= (int)1e6;
        timeout.tv_sec += (int)interval / 1e6;
    }
    return timeout;
}
