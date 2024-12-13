
#include "includes/arg_parser.h"

int parse_args(int ac, char **av, arg_parser_t *args)
{
    int opt;

    if (!args)
        return 1;

    args->ttl = 120;
    args->flags = 0x0;
    args->interval = 0;

    struct option long_options[] = {
        {"ttl", no_argument, NULL, 't'},
        {"ip-timestamp", no_argument, NULL, 'I'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}};

    while ((opt = getopt_long(ac, av, "vVhflnw:W:prsTt:I?", long_options, NULL)) != -1)
    {
        switch (opt)
        {

        case 'f':
            args->flags |= PING_FLOOD;
            break;
        case 'l':
            args->flags |= PING_PRELOAD;
            break;
        case 'n':
            args->flags |= PING_NO_DNS;
            break;
        case 't':
            args->ttl = 0;
            break;
        case 'v':
            args->flags |= PING_VEBROSE;
            break;
        case 'w':
            break;
        case 'W':
            break;
        case 'V':
            print_version();
            exit(0);
        case '?':
        case 'h':
            print_helper();
            exit(0);
        }
    }
    if (ac - optind != 1)
    {
        printf("%s: usage error: Destination address required\n", TARGET_NAME);
        exit(1);
    }
    args->hostname = av[optind];
    return 0;
}

void print_version()
{
    printf("%s version 1.0.0\n", TARGET_NAME);
}

void print_helper()
{
    printf("\nUsage\n"
           "ping [options] <destination>\n"
           "Options:\n"
           "  <destination>      dns name or ip address\n"
           "  -f                 flood ping\n"
           "  -h --help          print help and exit\n"
           "  -l <preload>       send <preload> number of packages while waiting replies\n"
           "  -n                 no dns name resolution\n"
           "  -t --ttl           define time to live\n"
           "  -v                 verbose output\n"
           "  -V                 print version and exit\n"
           "  -w <deadline>      reply wait <deadline> in seconds\n"
           "  -W <timeout>       time to wait for response\n");
}