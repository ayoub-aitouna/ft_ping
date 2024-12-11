
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

icmp_packet_t *recieve_icmp_packet(int sock)
{
    struct sockaddr_in from;
    struct iphdr *ip_hdr;
    struct timespec sleep_time;
    icmp_packet_t *icmp_packet;
    socklen_t from_len;
    char buffer[1024];

    icmp_packet = malloc(sizeof(icmp_packet_t) * 1);
    
    if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len) < 0)
    {
        perror("recvfrom has Failed");
        return NULL;
    }

    ip_hdr = (struct iphdr *)buffer;
    memcpy(icmp_packet, buffer + ip_hdr->ihl * 4, sizeof(icmp_packet_t));
    print_icmp_packet(icmp_packet, "recieved_icmp_packet");
    return icmp_packet;
}
