#pragma once

#include "../../includes/types.h"

void handle_exit(char *msg, short vebrose);
double get_round_time(struct timespec start);
icmp_packet_t build_icmp_packet(int seq);
uint16_t checksum(void *packet, int packet_len);
char *reverse_dns_lookup(struct sockaddr_in *addr);
short is_time_to_send(arg_parser_t args, double next_ts);
double get_timestamp();
struct sockaddr_in dns_lookup(char *str_addr);
void update_next_date(struct timeval *next, double interval);
int time_cmp(struct timeval *next);
struct timeval get_timeout(double interval);