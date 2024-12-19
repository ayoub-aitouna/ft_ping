#pragma once

#include "./types.h"
#include <sys/time.h>

double get_round_time(struct timespec start);
short is_time_to_send(ping_config_t args, double next_ts);
double get_timestamp();
void update_next_date(struct timeval *next, double interval);
int time_cmp(struct timeval *next);
struct timeval get_timeout(double interval);