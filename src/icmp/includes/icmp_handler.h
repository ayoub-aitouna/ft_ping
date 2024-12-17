#pragma once
#include "../../includes/types.h"
#include "../../utils/includes/utils.h"
#include "../../utils/includes/report.h"

int send_icmp_packet(int sock, struct sockaddr_in dest_addr, icmp_packet_t icmp_packet);
icmp_packet_t *recieve_icmp_packet(int sock);
int proccess_send(arg_parser_t *args, statistics_t *statics, struct timeval *timeout, int sockfd, int seq);