#include "ft_traceroute.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>


void set_socket_ttl(uint8_t ttl) {
    if (setsockopt(g_ft_traceroute.udp_sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        fprintf(stderr, "%s: setsockopt: %s\n", PROG_NAME, strerror(errno));
        exit_traceroute(EXIT_FAILURE);
    }
}

void init_udp_socket(void) {
    if ((g_ft_traceroute.udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "%s: socket: %s\n", PROG_NAME, strerror(errno));
        exit_traceroute(EXIT_FAILURE);
    }

    set_socket_ttl(g_ft_traceroute.options.ttl);
}

void init_icmp_socket(void) {
    if ((g_ft_traceroute.icmp_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        fprintf(stderr, "%s: socket: %s\n", PROG_NAME, strerror(errno));
        exit_traceroute(EXIT_FAILURE);
    }

    struct timeval tv_sock_timeout;
    tv_sock_timeout.tv_sec = g_ft_traceroute.options.wait;
    tv_sock_timeout.tv_usec = g_ft_traceroute.options.wait ? 0 : 1;
    if (setsockopt(g_ft_traceroute.icmp_sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_sock_timeout, sizeof(tv_sock_timeout)) < 0) {
        fprintf(stderr, "%s: setsockopt: %s\n", PROG_NAME, strerror(errno));
        exit_traceroute(EXIT_FAILURE);
    }
}