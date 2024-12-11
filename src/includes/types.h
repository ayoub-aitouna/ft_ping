#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PING_SLEEP_RATE 10000
#define LOG 0
#define TARGET_NAME "ft_ping"


#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

typedef struct icmphdr icmphdr_t;
typedef struct icmp_payload
{
    struct timespec timestamp;
    char msg[40];
} icmp_payload_t;

typedef struct icmp_packet
{
    icmphdr_t icmp_hdr;
    icmp_payload_t payload;
} icmp_packet_t;
