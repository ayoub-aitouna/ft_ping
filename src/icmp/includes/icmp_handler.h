#pragma once

#include "../../includes/types.h"

int send_icmp_packet(int sock, char *addr, int seq);
int recieve_icmp_packet(int sock, int ttl, char *addr, int seq, int bytes);
void exit_with_msg(char *msg);
struct timeval get_round_time(struct timeval old);