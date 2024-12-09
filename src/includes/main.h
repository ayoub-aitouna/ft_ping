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

#define TARGET_NAME "ft_ping"


typedef struct icmphdr t_icmp;
