#include "../include/ping_report.h"

void ping_report(double rtt, int seq, ping_config_t args)
{

    if (args.flags & PING_FLOOD)
    {
        printf(CLEAR_CHAR);
        return;
    }
    printf("%d bytes from ", args.packet_size + 8);
    printf("%s", args.flags & PING_NO_REVERSE_DNS ? args.ip : args.hostname);
    if (args.ip_type != ADRESS_IP4 && !(args.flags & PING_NO_REVERSE_DNS))
        printf(" (%s) ", args.ip);
    printf(" icmp_seq=%d ttl=%d time= %.2f ms\n", seq, args.ttl, rtt);
}

void ping_start_msg(ping_config_t args)
{
    // ip-header(20) + icmp-header(8) + icmp-payload(56)
    int total_size = args.packet_size + 28;
    char wb[1024];
    memset(wb, 0, sizeof(wb));
    if (args.ip_type != ADRESS_IP4 && !(args.flags & PING_NO_REVERSE_DNS))
        sprintf(wb, " (%s) ", args.ip);
    printf("%s %s%s %d(%d) bytes of data.\n", TARGET_NAME, args.flags & PING_NO_REVERSE_DNS ? args.ip : args.hostname, wb, args.packet_size, total_size);
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
    printf("| ICMP Payload: \n%s\n", packet->payload);
    printf("└---------------------------------------------------------------------┘\n");
}

double calculate_ping_stats(ping_stats_t *ping_stats)
{
    ping_stats->mdev = 0.0; // calculate it later
    int lost_count = ping_stats->sent - ping_stats->recieved;
    ping_stats->lost = ((float)lost_count / ping_stats->sent) * 100;
    ping_stats->avg = ping_stats->sum_rrt / ping_stats->recieved;
}

void update_ping_stats(ping_stats_t *statics, double rtt)
{
    if (rtt > statics->max)
        statics->max = rtt;
    if (rtt < statics->min)
        statics->min = rtt;
    statics->recieved += 1;
    statics->sum_rrt += rtt;
}

void static_report(ping_config_t args, ping_stats_t statics)
{
    calculate_ping_stats(&statics);
    printf("\n");
    printf("--- %s ping ping_stats ---\n", args.hostname);
    printf("%d packets transmitted, %d received, %.5f%% packet loss, time %.fms\n",
           statics.sent, statics.recieved, statics.lost, statics.total_rrt);
    if (statics.recieved)
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
               statics.min, statics.avg, statics.max, statics.mdev);
}
