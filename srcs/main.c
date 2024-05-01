#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#include "ft_traceroute.h"


t_ft_traceroute g_ft_traceroute = {
    .options = {
        .ttl = DEFAULT_FIRST_HOP,
        .max_hop = DEFAULT_MAX_HOP,
        .tries = DEFAULT_TRIES,
        .wait = DEFAULT_WAIT,
        .port = DEFAULT_PORT,
    },
    .host_arg_name = NULL,
    .udp_sockfd = -1,
    .icmp_sockfd = -1,
    .address_type = NONE,

    .res = NULL,
    // .sa4,
};


address_type get_address_info(const char *arg) {
    struct sockaddr_in6 sa6;
    struct addrinfo hints;

    memset(&g_ft_traceroute.sa4, 0, sizeof(g_ft_traceroute.sa4));
    g_ft_traceroute.sa4.sin_family = AF_INET;
    if (inet_pton(AF_INET, arg, &g_ft_traceroute.sa4.sin_addr) == 1) {
        return IPV4;
    }

    if (inet_pton(AF_INET6, arg, &sa6.sin6_addr) == 1) {
        return IPV6;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    if (getaddrinfo(arg, NULL, &hints, &g_ft_traceroute.res) == 0) {
        memset(&g_ft_traceroute.sa4, 0, sizeof(g_ft_traceroute.sa4));
        g_ft_traceroute.sa4.sin_family = AF_INET;
        g_ft_traceroute.sa4.sin_addr = ((struct sockaddr_in *)g_ft_traceroute.res->ai_addr)->sin_addr;
        return FQDN;
    }

    return NONE;
}


int main(int argc, char **argv) {
    get_options(argc, argv);

    if (optind == argc) {
        exit_missing_host(64);
    }
    g_ft_traceroute.host_arg_name = argv[optind];

    g_ft_traceroute.address_type = get_address_info(g_ft_traceroute.host_arg_name);
    if (g_ft_traceroute.address_type == IPV6) {
        exit_ipv6_support(2);
    } else if (g_ft_traceroute.address_type == NONE) {
        exit_unknown_host(2);
    }

    traceroute_loop();

    exit_traceroute(EXIT_SUCCESS);
}