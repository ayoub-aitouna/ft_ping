#include "includes/main.h"

void exit_with_msg(char *msg, int status)
{
    printf("%s terminated for the following reasons\n", TARGET_NAME);
    printf("%s\n", msg);
    exit(status);
}

int main()
{
    
    int sock;
    struct protoent *protocol;

    protocol = getprotobyname("icmp");
    sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0)
        exit_with_msg("Failed while trying to open a raw socket", 1);

    return 0;
}