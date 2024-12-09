#include "includes/main.h"

void exit_with_msg(char *msg, int status)
{
    printf("%s terminated for the following reasons\n", TARGET_NAME);
    printf("%s\n", msg);
    exit(status);
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

t_icmp build_icmp_packet()
{
    t_icmp icmp_hdr;
    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.un.echo.id = getpid();
    icmp_hdr.un.echo.sequence = 1;
    icmp_hdr.checksum = 0;
    icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
    printf("Created New ICMP Header:\n");
    printf("Type: %d, Code: %d, ID: %d, Sequence: %d, Checksum: 0x%x\n",
           icmp_hdr.type, icmp_hdr.code, icmp_hdr.un.echo.id,
           icmp_hdr.un.echo.sequence, icmp_hdr.checksum);
    return icmp_hdr;
}

int main()
{

    int sock;
    struct protoent *protocol;
    t_icmp icmp_hdr;
    protocol = getprotobyname("icmp");
    sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0)
        exit_with_msg("Failed while trying to open a raw socket", 1);
    icmp_hdr = build_icmp_packet();
    return 0;
}