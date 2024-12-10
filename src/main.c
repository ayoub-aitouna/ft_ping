#include "includes/main.h"

int ping_loop = 1;

void exit_with_msg(char *msg)
{
    printf("%s terminated for the following reasons\n", TARGET_NAME);
    perror(msg);
    exit(EXIT_FAILURE);
}

void inthandler()
{
    ping_loop = 0;
}

void ping(int sock, int ttl, char *addr)
{
    struct timeval st;
    int bytes;
    int seq;
    int sp, rp;

    seq = 0;
    gettimeofday(&st, NULL);

    while (ping_loop)
    {
        if ((bytes = send_icmp_packet(sock, addr, seq)) > 0)
            sp++;

        if (recieve_icmp_packet(sock, ttl, addr, seq, bytes) > 0)
            rp++;

        seq++;
        usleep(PING_SLEEP_RATE);
    }

    struct timeval diff = get_round_time(st);
    printf("\n--- %s ping statistics --- \n", addr);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %ld ms\n", seq, rp, sp - rp, diff.tv_usec / 1000);
    printf("rtt min/avg/max/mdev = 20.448/20.886/21.324/0.438 ms\n");
}

int main(int ac, char **av)
{

    struct protoent *protocol;
    int sock;
    int ttl;

    if (ac != 2)
        exit_with_msg("Incorrect number of arguments");

    signal(SIGINT, inthandler);
    protocol = getprotobyname("icmp");
    ttl = 60;

    if ((sock = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0)
        exit_with_msg("Failed while trying to open a raw socket");

    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
        exit_with_msg("Failed while trying to Set TTL using setsockopt");

    ping(sock, ttl, av[1]);

    close(sock);
    return 0;
}