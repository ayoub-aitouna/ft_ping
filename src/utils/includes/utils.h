#pragma once

#include "../../includes/types.h"

void ping_report(double rtt, int ttl, int bytes, struct sockaddr_in dest_addr, int seq);
void handle_exit(char *msg);
double get_round_time(struct timespec start);
icmp_packet_t build_icmp_packet(int seq);
void print_icmp_packet(icmp_packet_t *packet, char *title);
uint16_t checksum(void *packet, int packet_len);
char *reverse_dns_lookup(struct sockaddr_in *addr);