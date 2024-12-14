#pragma once

#include "../../includes/types.h"

void ping_report(double rtt, int bytes, int seq, arg_parser_t args);
void handle_exit(char *msg, short vebrose);
double get_round_time(struct timespec start);
icmp_packet_t build_icmp_packet(int seq);
void print_icmp_packet(icmp_packet_t *packet, char *title);
uint16_t checksum(void *packet, int packet_len);
char *reverse_dns_lookup(struct sockaddr_in *addr);
short is_time_to_send(arg_parser_t args, double next_ts);
double get_timestamp();
double calculate_statistics(statistics_t *statistics);
struct sockaddr_in dns_lookup(char *str_addr);
void ping_start_msg(arg_parser_t args, int total_size, int data_size);