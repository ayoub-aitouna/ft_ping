#pragma once
#include "./types.h"
#include "./network_io.h"
#include "./utils.h"
#include "./arg_parser.h"
#include "./ping_report.h"
#include "./time_utils.h"

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

void proccess_events(int epoll_fd, int sockfd, ping_config_t args);
void inthandler();
int setup_epoll(int sock);
int setup_socket(int ttl);
