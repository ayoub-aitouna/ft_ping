
#include "includes/icmp_handler.h"

int send_icmp_packet(int sock, struct sockaddr_in dest_addr, icmp_packet_t icmp_packet)
{
    if (sendto(sock, &icmp_packet, sizeof(icmp_packet), 0,
               (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto has Failed");
        return -1;
    }
    print_icmp_packet(&icmp_packet, "sent_icmp_packet");
    return sizeof(icmp_packet);
}

void print_ip_address(uint32_t saddr)
{
    struct in_addr ip_addr;
    ip_addr.s_addr = saddr;
    printf("Source IP Address: %s\n", inet_ntoa(ip_addr));
}

icmp_packet_t *recieve_icmp_packet(int sock)
{
    struct sockaddr_in from;
    struct iphdr *ip_hdr;
    struct timespec sleep_time;
    icmp_packet_t *icmp_packet;
    socklen_t from_len;
    char buffer[1024];
    char ip_str[INET_ADDRSTRLEN];

    icmp_packet = malloc(sizeof(icmp_packet_t));
    if (read(sock, buffer, sizeof(buffer)) < 0)
    {
        perror("recvfrom has Failed");
        return NULL;
    }

    ip_hdr = (struct iphdr *)buffer;
    memcpy(icmp_packet, buffer + ip_hdr->ihl * 4, sizeof(icmp_packet_t));

    inet_ntop(AF_INET, &(from.sin_addr), ip_str, INET_ADDRSTRLEN);
    print_icmp_packet(icmp_packet, "recieved_icmp_packet");
    return icmp_packet;
}

int proccess_send(arg_parser_t *args, statistics_t *statics, struct timeval *timeout, int sockfd, int seq)
{
    int bytes;

    if (args->count != -1 && statics->sent >= args->count)
        return -3;
    if ((args->timeout != 0 && time_cmp(timeout)))
        return -2;
    if ((bytes = send_icmp_packet(sockfd, args->dest_addr, build_icmp_packet(seq))) < 0)
        return -1;
    if (args->flags & PING_FLOOD)
        printf(FLOOD_INDECATOR);
    statics->sent += 1;
    return 0;
}
