#include "includes/utils.h"
#include <sys/time.h>

icmp_packet_t build_icmp_packet(int seq)
{
    icmp_packet_t packet;

    memset(&packet, 0, sizeof(packet));

    packet.icmp_hdr.type = ICMP_ECHO;
    packet.icmp_hdr.code = 0;
    packet.icmp_hdr.un.echo.id = getpid();
    packet.icmp_hdr.un.echo.sequence = seq;
    packet.icmp_hdr.checksum = 0;
    memset(packet.payload.msg, 0, sizeof(packet.payload.msg));
    for (int i = 0; i < 40; i++)
        packet.payload.msg[i] = '-';
    clock_gettime(CLOCK_MONOTONIC, &packet.payload.timestamp);
    packet.icmp_hdr.checksum = checksum(&packet, sizeof(packet));
    return packet;
}

uint16_t checksum(void *packet, int packet_len)
{
    uint16_t *buff = packet;
    u_int32_t sum = 0;
    u_int16_t result = 0;

    while (packet_len > 1)
    {
        sum += *buff++;
        packet_len -= 2;
    }

    if (packet_len == 1)
        sum += *(u_int8_t *)buff;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

double get_round_time(struct timespec start)
{
    struct timespec end;
    struct timespec diff;

    clock_gettime(CLOCK_MONOTONIC, &end);
    return (float)(end.tv_sec - start.tv_sec) * 1000 +
           (float)(end.tv_nsec - start.tv_nsec) / 1000000;
}

char *reverse_dns_lookup(struct sockaddr_in *addr)
{
    char *buffer;

    buffer = malloc(sizeof(char) * NI_MAXHOST);
    if (getnameinfo((struct sockaddr *)addr, sizeof(struct sockaddr_in), buffer, NI_MAXHOST, NULL, 0, NI_NAMEREQD))
        handle_exit("getnameinfo fialed", 0);
    return buffer;
}

struct sockaddr_in dns_lookup(char *hostname)
{
    struct sockaddr_in dest_addr;
    struct in_addr **addr_list;
    struct hostent *hent;
    char *ip;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;

    if (!(hent = gethostbyname(hostname)))
        handle_exit("Name or service not known", 0);

    addr_list = (struct in_addr **)hent->h_addr_list;

    for (int i = 0; addr_list[i] != NULL; i++)
        ip = inet_ntoa(*addr_list[i]);

    dest_addr.sin_addr.s_addr = inet_addr(ip);
    return dest_addr;
}

double get_timestamp()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec +
           time.tv_nsec / 1000000000;
}

short is_time_to_send(arg_parser_t args, double next_ts)
{
    if (args.flags & PING_FLOOD)
        return get_timestamp() + args.interval > next_ts - 1;
    return get_timestamp() > next_ts;
}

void handle_exit(char *msg, short vebrose)
{
    if (!vebrose)
    {
        printf("%s terminated for the following reasons\n", TARGET_NAME);
        perror(msg);
    }
    else
        printf("Failed\n");
    exit(EXIT_FAILURE);
}

void update_next_date(struct timeval *next, double interval)
{
    next->tv_usec += interval * 1e6;
    if (next->tv_usec >= 1e6)
    {
        next->tv_usec -= 1e6;
        next->tv_sec++;
    }
}

int time_cmp(struct timeval *next)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return timercmp(&now, next, >=);
}

struct timeval get_timeout(double interval)
{
    struct timeval timeout;
    gettimeofday(&timeout, NULL);
    timeout.tv_sec += (int)interval;
    timeout.tv_usec += (int)(interval * 1e6);
    if (timeout.tv_usec >= 1e6)
    {
        timeout.tv_usec %= (int)1e6;
        timeout.tv_sec += (int)interval / 1e6;
    }
    return timeout;
}
