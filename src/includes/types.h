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

enum PING_FLAGS
{
    PING_SENT = 0x0,
#define PING_SENT PING_SENT
    PING_RECVED = 0x1 << 1,
#define PING_RECVED PING_RECVED
};

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

enum ARGUMENT_FLAGS
{
    PING_VEBROSE = 0x1,
#define PING_VEBROSE PING_VEBROSE
    PING_FLOOD = 0x1 << 1,
#define PING_FLOOD PING_FLOOD
    PING_PRELOAD = 0x1U << 2,
#define PING_PRELOAD PING_PRELOAD
    PING_NO_DNS = 0x1U << 3,
#define PING_NO_DNS PING_NO_DNS
};

typedef struct arg_parser
{
    char *hostname;
    char *rdns_hostname;
    char ip[INET_ADDRSTRLEN];
    __uint32_t flags;
    double interval;
    __uint32_t ttl;
    struct sockaddr_in dest_addr;

} arg_parser_t;

typedef struct statistics
{
    double min;
    double avg;
    double max;
    double mdev;
    __uint32_t sent;
    __uint32_t recieved;
    double lost;
} statistics_t;