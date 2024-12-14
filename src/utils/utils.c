#include "includes/utils.h"

void ping_report(double rtt, int bytes, int seq, arg_parser_t args)
{
    char wb[1024];
    memset(wb, 0, sizeof(wb));
    if (args.flags & PING_FLOOD)
    {
        printf("\b");
        return;
    }
    if (args.ip_type != ADRESS_IP4)
        sprintf(wb, " (%s) ", args.ip);
    printf("%d bytes from %s%s icmp_seq=%d ttl=%d time= %.2f ms\n",
           bytes, args.hostname, wb, seq, args.ttl, rtt);
}

void ping_start_msg(arg_parser_t args, int total_size, int data_size)
{
    char wb[1024];
    memset(wb, 0, sizeof(wb));
    if (args.ip_type != ADRESS_IP4)
        sprintf(wb, " (%s) ", args.ip);
    printf("%s %s%s %d(%d) bytes of data.\n", TARGET_NAME, args.hostname, wb, data_size, total_size);
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

void print_icmp_packet(icmp_packet_t *packet, char *title)
{
    if (!LOG)
        return;
    printf("┌------------------------------%s------------------------┐\n", title);
    printf("| ICMP Header:\n");
    printf("| Type: %d, Code: %d, ID: %d, Sequence: %d, Checksum: 0x%x\n",
           packet->icmp_hdr.type, packet->icmp_hdr.code, packet->icmp_hdr.un.echo.id,
           packet->icmp_hdr.un.echo.sequence, packet->icmp_hdr.checksum);

    printf("|---------------------------------------------------------------------|\n");

    printf("| ICMP Payload:\n");
    printf("| Time: %06ld:%06ld ms \n",
           packet->payload.timestamp.tv_sec,
           packet->payload.timestamp.tv_nsec);
    printf("└---------------------------------------------------------------------┘\n");
}

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

// 1 / 1 = 1
double calculate_statistics(statistics_t *statistics)
{
    statistics->mdev = 0.0; // calculate it later
    statistics->lost = (float)((statistics->sent - statistics->recieved) / statistics->sent) * 100;
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
