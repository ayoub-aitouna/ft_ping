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
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#define PING_SLEEP_RATE 1000000

typedef struct icmphdr icmphdr_t;
typedef struct icmp_payload
{
    struct timeval timestamp;
    char msg[40];
} icmp_payload_t;

typedef struct icmp_packet
{
    icmphdr_t icmp_hdr;
    icmp_payload_t payload;
} icmp_packet_t;
