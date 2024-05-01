#include "ft_traceroute.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>


// print functions
static void printerr_missing_host(void) {
    fprintf(stderr, "%s\n", TRACEROUTE_MISSING_HOST);
}

static void printerr_traceroute_error_usage(void) {
    fprintf(stderr, "%s\n", TRACEROUTE_ERROR_USAGE);
}

static void print_traceroute_help(void) {
    printf("%s\n", TRACEROUTE_HELP);
}

static void print_traceroute_usage(void) {
    printf("%s\n", TRACEROUTE_USAGE);
}

static void printerr_ipv6_not_supported(void) {
    fprintf(stderr, "%s\n", TRACEROUTE_IPV6_SUPPORT);
}

static void printerr_unknown_host(void) {
    fprintf(stderr, "%s\n", TRACEROUTE_UNKNOWN_HOST);
}


// exit functions
void exit_traceroute(int ret) {
    if (g_ft_traceroute.res) {
        freeaddrinfo(g_ft_traceroute.res);
    }

    if (g_ft_traceroute.udp_sockfd >= 0) {
        close(g_ft_traceroute.udp_sockfd);
    }

    if (g_ft_traceroute.icmp_sockfd >= 0) {
        close(g_ft_traceroute.icmp_sockfd);
    }

    exit(ret);
}

void exit_missing_host(int ret) {
    printerr_missing_host();
    printerr_traceroute_error_usage();
    exit_traceroute(ret);
}

void exit_error_usage(int ret) {
    printerr_traceroute_error_usage();
    exit_traceroute(ret);
}

void exit_help(int ret) {
    print_traceroute_help();
    exit_traceroute(ret);
}

void exit_usage(int ret) {
    print_traceroute_usage();
    exit_traceroute(ret);
}

void exit_ipv6_support(int ret) {
    printerr_ipv6_not_supported();
    exit_traceroute(ret);
}

void exit_unknown_host(int ret) {
    printerr_unknown_host();
    exit_traceroute(ret);
}