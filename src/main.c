#include "includes/main.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

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
    ping_start_msg(args);
    proccess_events(epoll_fd, sock, args);
    close(epoll_fd);
    close(sock);
    return EXIT_SUCCESS;
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
    struct timeval next, now, timeout, gracfull;
    icmp_packet_t *packet;
    __uint32_t bytes, seq;
    struct timespec start, record[MAX_SEQ_REC];
    statistics_t statics;
    __uint32_t fds;
    double rtt;
    int send_status;

    seq = 1;
    clock_gettime(CLOCK_MONOTONIC, &start);
    statics.min = 1e9;
    statics.sent = 0;
    statics.recieved = 0;
    statics.sum_rrt = 0;
    gracfull.tv_sec = 0;
    gracfull.tv_usec = 0;

    next.tv_sec = 0;
    next.tv_usec = 0;

    timeout = get_timeout(args.timeout);
    while (running)
    {
        gettimeofday(&now, 0);
        if (next.tv_sec == 0)
        {
            next = now;
            next.tv_usec += args.interval * 1e6;
        }
        if ((fds = epoll_wait(epoll_fd, events, 1, 0)) < 0)
            handle_exit("epoll wait failed .", 0);
        if (fds == 0)
        {
            printf("Timout wailting for packet. \n");
            continue;
        }
        if ((events->events & EPOLLIN))
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
            rtt = get_round_time(record[seq % MAX_SEQ_REC]);
            ping_report(rtt, seq, args);
            update_statistics(&statics, rtt);
            seq++;
            free(packet);
        }
        if (gracfull.tv_sec != 0 && (time_cmp(&gracfull) || statics.sent == statics.recieved))
            break;
        if (time_cmp(&next))
        {
            do
            {
                clock_gettime(CLOCK_MONOTONIC, &record[(statics.sent + 1) % MAX_SEQ_REC]);
                send_status = proccess_send(&args, &statics, &timeout, sockfd, seq);
            } while (args.flags & PING_PRELOAD && statics.sent < args.preload);
            if (send_status == -3)
            {
                if (gracfull.tv_sec == 0)
                    gracfull = get_timeout(1);                
                continue;
            }
            if (send_status == -2)
                break;
            if (send_status == -1)
                continue;
            args.flags &= ~PING_PRELOAD;
            update_next_date(&next, args.interval);
        }
    }
    statics.total_rrt = get_round_time(start);
    static_report(args, statics);
}
