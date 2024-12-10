
#include "includes/icmp_handler.h"

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

void print_icmp_packet(icmp_packet_t *packet)
{
    printf("ICMP Header:\n");
    printf("Type: %d, Code: %d, ID: %d, Sequence: %d, Checksum: 0x%x\n",
           packet->icmp_hdr.type, packet->icmp_hdr.code, packet->icmp_hdr.un.echo.id,
           packet->icmp_hdr.un.echo.sequence, packet->icmp_hdr.checksum);

    printf("ICMP Payload:\n");
    printf("Time: %06ld:%06ld ms \n",
           packet->payload.timestamp.tv_sec,
           packet->payload.timestamp.tv_usec);
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
    gettimeofday(&packet.payload.timestamp, NULL);
    packet.icmp_hdr.checksum = checksum(&packet, sizeof(packet));
    return packet;
}

struct timeval get_round_time(struct timeval old)
{
    struct timeval new;
    gettimeofday(&new, NULL);
    int sec_diff = new.tv_sec - old.tv_sec;
    int usec_diff = new.tv_usec - old.tv_usec;
    return (struct timeval){.tv_sec = sec_diff, usec_diff = usec_diff};
}

void ping_report(struct timeval RTT, int ttl, int bytes, char *addr, int seq)
{
    long ms;
    long remainig;

    ms = RTT.tv_usec / 1000;
    remainig = (RTT.tv_usec % 1000) / 10;
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time= %02ld.%02ld ms\n",
           bytes, addr, seq, ttl, ms, remainig);
}

int send_icmp_packet(int sock, char *addr, int seq)
{
    struct sockaddr_in dest_addr;
    icmp_packet_t icmp_packet;
    icmphdr_t *icmp_hdr;

    icmp_packet = build_icmp_packet(seq);

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(addr);

    if (sendto(sock, &icmp_packet, sizeof(icmp_packet), 0,
               (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto has Failed");
        return -1;
    }

    return sizeof(icmp_packet);
}

int recieve_icmp_packet(int sock, int ttl, char *addr, int seq, int bytes)
{
    icmp_packet_t *received_packet;
    struct sockaddr_in from;
    struct iphdr *ip_hdr;
    socklen_t from_len;
    char buffer[4024];

    if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len) < 0)
    {
        perror("recvfrom has Failed");
        return -1;
    }
    ip_hdr = (struct iphdr *)buffer;
    received_packet = (icmp_packet_t *)(buffer + ip_hdr->ihl * 4);
    ping_report(get_round_time(received_packet->payload.timestamp), ttl, bytes, addr, seq);
}