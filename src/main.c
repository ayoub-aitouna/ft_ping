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
    setvbuf(stdout, NULL, _IONBF, 0);

    parse_args(ac, av, &args);

    sock = setup_socket(args.ttl);
    epoll_fd = setup_epoll(sock);
    ping_start_msg(args, 84, 56);
    proccess_events(epoll_fd, sock, args);

    close(epoll_fd);
    close(sock);

    return EXIT_SUCCESS;
}

void set_nonblocking(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
        handle_exit("fcntl get failed", 0);

    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
        handle_exit("fcntl set failed", 0);
}

int setup_socket(int ttl)
{
    struct protoent *protocol;
    int sock;
    int flags;

    protocol = getprotobyname("icmp");
    if ((sock = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0)
        handle_exit("Failed while trying to open a raw socket", 0);

    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
        handle_exit("Failed while trying to Set TTL using setsockopt", 0);

    set_nonblocking(sock);
    return sock;
}

int setup_epoll(int sock)
{
    struct epoll_event ev;
    int epoll_fd;

    if ((epoll_fd = epoll_create1(0)) < 0)
    {
        close(epoll_fd);
        handle_exit("epoll create failed. ", 0);
    }

    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = sock;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0)
    {
        close(epoll_fd);
        handle_exit("epoll ctl failed .", 0);
    }

    return epoll_fd;
}

void inthandler()
{
    running = false;
}

void proccess_events(int epoll_fd, int sockfd, arg_parser_t args)

{
    struct epoll_event ev, events[1];
    double next_ts, rtt;
    icmp_packet_t *packet;
    __uint32_t bytes, seq;
    __uint32_t fds;
    statistics_t statics;
    struct timespec start;

    seq = 1;
    clock_gettime(CLOCK_MONOTONIC, &start);
    next_ts = get_timestamp();
    statics.min = 1e9;
    statics.sent = 0;
    statics.recieved = 0;

    while (running)
    {
        fds = epoll_wait(epoll_fd, events, 1, 0);
        if (fds < 0)
            handle_exit("epoll wait failed .", 0);

        else if (fds == 0)
        {
            printf("Timout wailting for packet. \n");
            continue;
        }

        if (events[0].data.fd == sockfd && (events->events & EPOLLOUT) && is_time_to_send(args, next_ts))
        {
            if (args.count != -1 && statics.sent >= args.count)
                break;
            next_ts += args.interval < 0.01 ? 1 : args.interval;
            if ((bytes = send_icmp_packet(sockfd, args.dest_addr, build_icmp_packet(seq))) < 0)
            {
                printf("snd failed \n");
                continue;
            }
            if (args.flags & PING_FLOOD)
                printf(".");
            statics.sent += 1;
        }

        if (events[0].data.fd == sockfd && (events->events & EPOLLIN))
        {
            if (!(packet = recieve_icmp_packet(sockfd)))
                continue;

            if (packet->icmp_hdr.type != ICMP_ECHOREPLY || packet->icmp_hdr.code != 0)
            {
                if (packet->icmp_hdr.type == ICMP_TIME_EXCEEDED && packet->icmp_hdr.code == 0)
                {
                    printf("From %s icmp_seq=%d Time to live exceeded\n", args.hostname, seq);
                    seq++;
                }
                free(packet);
                continue;
            }

            rtt = get_round_time(packet->payload.timestamp);
            ping_report(rtt, bytes, seq, args);
            update_statistics(&statics, rtt);
            seq++;
            free(packet);
        }
    }

    statics.total_rrt = get_round_time(start);
    static_report(args, statics);
}

void update_statistics(statistics_t *statics, double rtt)
{
    if (rtt > statics->max)
        statics->max = rtt;
    if (rtt < statics->min)
        statics->min = rtt;
    statics->recieved += 1;
}

void static_report(arg_parser_t args, statistics_t statics)
{
    calculate_statistics(&statics);
    printf("\n");
    printf("--- %s ping statistics ---\n", args.hostname);
    printf("%d packets transmitted, %d received, %.3f%% packet loss, time %.fms\n",
           statics.sent, statics.recieved, statics.lost, statics.total_rrt);
    if (statics.recieved)
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
               statics.min, statics.avg, statics.max, statics.mdev);
}