#pragma once

#include "./types.h"
#include "./utils.h"
#include "./ping_report.h"
#include "./time_utils.h"

int send_icmp_packet(int sock, struct sockaddr_in dest_addr, icmp_packet_t icmp_packet);
icmp_packet_t *recieve_icmp_packet(int sock);
int proccess_send(ping_config_t *args, ping_stats_t *statics, struct timeval *timeout, int sockfd, int seq);