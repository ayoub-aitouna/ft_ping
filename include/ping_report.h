#pragma once

#include "./types.h"

void ping_report(double rtt, int seq, ping_config_t args);
void print_icmp_packet(icmp_packet_t *packet, char *title);
void ping_start_msg(ping_config_t args);
double calculate_ping_stats(ping_stats_t *ping_stats);
void static_report(ping_config_t args, ping_stats_t statics);
void update_ping_stats(ping_stats_t *statics, double rtt);