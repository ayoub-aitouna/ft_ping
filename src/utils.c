#include "../include/utils.h"

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
