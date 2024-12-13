#include "includes/main.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int running = true;

int main(int ac, char **av)
{
    struct sockaddr_in dest_addr;
    arg_parser_t args;
    int sock, epoll_fd;

    signal(SIGINT, inthandler);

    if (parse_args(ac, av, &args))
        return 1;

    sock = setup_socket(args.ttl);
    args.dest_addr = dns_lookup(args.hostname);
    epoll_fd = setup_epoll(sock);

    args.rdns_hostname = reverse_dns_lookup(&args.dest_addr);
    inet_ntop(AF_INET, &(args.dest_addr.sin_addr), args.ip, INET_ADDRSTRLEN);

    proccess_events(epoll_fd, sock, args);

    close(epoll_fd);
    close(sock);
    free(args.rdns_hostname);
    return 0;
}

int setup_socket(int ttl)
{
    struct protoent *protocol;
    int sock;

    protocol = getprotobyname("icmp");
    if ((sock = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0)
        handle_exit("Failed while trying to open a raw socket");

    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
        handle_exit("Failed while trying to Set TTL using setsockopt");

    return sock;
}

int setup_epoll(int sock)
{
    struct epoll_event ev;
    int epoll_fd;

    if ((epoll_fd = epoll_create1(0)) < 0)
    {
        close(epoll_fd);
        handle_exit("epoll create failed. ");
    }

    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = sock;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0)
    {
        close(epoll_fd);
        handle_exit("epoll ctl failed .");
    }

    return epoll_fd;
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
        handle_exit("gethostbyname failed");

    addr_list = (struct in_addr **)hent->h_addr_list;

    for (int i = 0; addr_list[i] != NULL; i++)
        ip = inet_ntoa(*addr_list[i]);

    dest_addr.sin_addr.s_addr = inet_addr(ip);
    return dest_addr;
}

void inthandler()
{
    running = false;
}

void proccess_events(int epoll_fd, int sockfd, arg_parser_t args)

{
    struct epoll_event ev, events[1];
    icmp_packet_t *packet;
    __uint32_t bytes, seq;
    __uint32_t fds;
    __uint32_t flag;
    statistics_t statics;
    double next_ts, rtt;

    seq = 1;
    next_ts = get_timestamp();
    statics.min = 1e9;
    statics.sent = 0;
    statics.recieved = 0;
    while (running)
    {
        fds = epoll_wait(epoll_fd, events, 1, 5000);

        if (fds < 0)
            handle_exit("epoll wait failed .");

        else if (fds == 0)
        {
            printf("Timout wailting for packet. \n");
            continue;
        }

        if (events[0].data.fd = sockfd && (events->events & EPOLLOUT) && is_time_to_send(args, next_ts, flag))
        {
            next_ts += 1;
            if ((bytes = send_icmp_packet(sockfd, args.dest_addr, build_icmp_packet(seq))) < 0)
            {
                printf("snd failed \n");
                continue;
            }
            flag = PING_SENT;
            statics.sent += 1;
        }

        if (events[0].data.fd = sockfd && (events->events & EPOLLIN))
        {
            if (!(packet = recieve_icmp_packet(sockfd)))
            {
                printf("rcv failed \n");
                continue;
            }
            if (packet->icmp_hdr.code != ICMP_ECHOREPLY)
                continue;
            rtt = get_round_time(packet->payload.timestamp);

            ping_report(rtt, bytes, seq, args);
            free(packet);
            seq++;
            flag = PING_RECVED;
            statics.recieved += 1;
            if (rtt > statics.max)
                statics.max = rtt;
            if (rtt < statics.min)
                statics.min = rtt;
        }
    }
    static_report(args, statics);
}

void static_report(arg_parser_t args, statistics_t statcs)
{
    calculate_statistics(statcs);
    printf("\n");
    printf("--- %s ping statistics ---\n", args.rdns_hostname);
    printf("%d packets transmitted, %d received, %.3f%% packet loss, time 1002ms\n",
           statcs.sent, statcs.recieved, statcs.lost);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
           statcs.min, statcs.avg, statcs.max, statcs.mdev);
}