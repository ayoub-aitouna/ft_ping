# ft_ping

uses the ICMP protocol's mandatory ECHO_REQUEST datagram to elicit an ICMP ECHO_RESPONSE from a host or
gateway. ECHO_REQUEST datagrams ("pings") have an IP and ICMP header, followed by a struct timeval and then an
arbitrary number of "pad" bytes used to fill out the packet.

[Documentation](https://www.notion.so/Implementing-rt-Protocol-in-C-157b441547cb8063878cfa0e27f2f022?pvs=4)
