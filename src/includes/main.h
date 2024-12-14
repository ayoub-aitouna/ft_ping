#pragma once
#include "types.h"
#include "../icmp/includes/icmp_handler.h"
#include "../utils/includes/utils.h"
#include "../utils/includes/arg_parser.h"
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

void proccess_events(int epoll_fd, int sockfd, arg_parser_t args);
void inthandler();
int setup_epoll(int sock);
int setup_socket(int ttl);
void static_report(arg_parser_t args, statistics_t statics);
void update_statistics(statistics_t *statics, double rtt);