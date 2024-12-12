#include "includes/main.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int running = true;

int main(int ac, char **av)
{
    signal(SIGINT, inthandler);

    struct sockaddr_in dest_addr;
    int sock, epoll_fd, ttl;

    ttl = 120;
    if (ac != 2)
        handle_exit("Incorrect number of arguments");

    sock = setup_socket(ttl);
    dest_addr = dns_lookup(av[1]);
    epoll_fd = setup_epoll(sock);

    proccess_events(epoll_fd, sock, ttl, dest_addr);

    close(epoll_fd);
    close(sock);

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

double get_timestamp()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec +
           time.tv_nsec / 1000000000;
}

void proccess_events(int epoll_fd, int sockfd, int ttl, struct sockaddr_in dest_addr)
{
    struct epoll_event ev, events[1];
    int bytes, seq;
    icmp_packet_t *packet;
    int fds, r_flag;

    seq = 1;
    double next_ts = get_timestamp();

    while (running)
    {
        r_flag = false;
        fds = epoll_wait(epoll_fd, events, 1, 5000);
        if (fds < 0)
            handle_exit("epoll wait failed .");

        else if (fds == 0)
        {
            printf("Timout wailting for packet. \n");
            continue;
        }

        if (events[0].data.fd = sockfd && (events->events & EPOLLOUT) && get_timestamp() > next_ts)
        {
            next_ts += 1;
            if ((bytes = send_icmp_packet(sockfd, dest_addr, build_icmp_packet(seq))) < 0)
            {
                printf("snd failed \n");
                continue;
            }
        }

        if (events[0].data.fd = sockfd && (events->events & EPOLLIN))
        {
            if (!(packet = recieve_icmp_packet(sockfd)))
            {
                printf("rcv failed \n");
                continue;
            }
            ping_report(get_round_time(packet->payload.timestamp), ttl, bytes, dest_addr, seq);
            r_flag = true;
            free(packet);
            seq++;
        }
    }
}