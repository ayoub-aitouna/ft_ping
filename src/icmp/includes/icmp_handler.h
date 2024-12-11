#pragma once
#include "../../includes/types.h"
#include "../../utils/includes/utils.h"

int send_icmp_packet(int sock, struct sockaddr_in dest_addr, icmp_packet_t icmp_packet);
icmp_packet_t *recieve_icmp_packet(int sock);
