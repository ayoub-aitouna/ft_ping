#pragma once

#include "../../includes/types.h"

void ping_report(double rtt, int seq, arg_parser_t args);
void print_icmp_packet(icmp_packet_t *packet, char *title);
void ping_start_msg(arg_parser_t args);
double calculate_statistics(statistics_t *statistics);
void static_report(arg_parser_t args, statistics_t statics);
void update_statistics(statistics_t *statics, double rtt);