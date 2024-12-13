#pragma once
#include "types.h"
#include "../icmp/includes/icmp_handler.h"
#include "../utils/includes/utils.h"
#include "../utils/includes/arg_parser.h"
#include <signal.h>
#include <errno.h>
#include <string.h>

void proccess_events(int epoll_fd, int sockfd, arg_parser_t args);
void toggle_epoll(int epoll_fd, int sockfd, uint32_t events);
void inthandler();
struct sockaddr_in dns_lookup(char *str_addr);
int setup_epoll(int sock);
int setup_socket(int ttl);
void static_report();
