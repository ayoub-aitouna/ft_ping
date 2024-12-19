#pragma once

#include "./types.h"

void handle_exit(char *msg, short vebrose);
icmp_packet_t build_icmp_packet(int seq);
uint16_t checksum(void *packet, int packet_len);
char *reverse_dns_lookup(struct sockaddr_in *addr);
struct sockaddr_in dns_lookup(char *str_addr);
