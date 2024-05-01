#include "ft_traceroute.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/time.h>
#include <errno.h>
#include <arpa/inet.h>


static void fill_data(void) {
    uint8_t     *data = (uint8_t *)g_ft_traceroute.packet;

    memcpy(data, "SUPERMAN", 8);
    data[8] = 0;
}

struct timeval *get_timeval_diff(const struct timeval *tv_start) {
    struct timeval  tv_end, *tv_diff = &(struct timeval){0, 0};

    gettimeofday(&tv_end, NULL);

    tv_diff->tv_sec = tv_end.tv_sec - tv_start->tv_sec;
    tv_diff->tv_usec = tv_end.tv_usec - tv_start->tv_usec;
    if (tv_diff->tv_usec < 0) {
        tv_diff->tv_sec--;
        tv_diff->tv_usec += 1000000;
    }

    return tv_diff;
}

static uint8_t  receive_packet(struct timeval *tv, uint8_t *is_ip_printed) {
    struct iphdr    *ip_reply;
    char    recv_time[64], ip_str[INET_ADDRSTRLEN], *needle;
    uint8_t ret = 0;

    // Receive packet
    ssize_t recv_len = recvfrom(g_ft_traceroute.icmp_sockfd, g_ft_traceroute.buffer, sizeof(g_ft_traceroute.buffer), 0, NULL, NULL);

    tv = get_timeval_diff(tv);
    sprintf(recv_time, "%ld.%ld", tv->tv_usec / 1000, tv->tv_usec - (tv->tv_usec / 1000));
    needle = strstr(recv_time, ".");
    for (int i = 1; i < 4; ++i) {
        if (*(needle + i) == 0) {
            needle[i] = '0';
        }
    }
    needle[4] = 0;

    if (recv_len < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
        fflush(stdout);
        fprintf(stderr, "%s: recvfrom: %s\n", PROG_NAME, strerror(errno));
        exit_traceroute(EXIT_FAILURE);
    } else if (recv_len < 0) {
        printf(" * ");
    } else {
        ip_reply = (struct iphdr *)g_ft_traceroute.buffer;

        if (!(*is_ip_printed)) {
            *is_ip_printed = 1;
            printf(" %s ", inet_ntoa(*(struct in_addr *)&ip_reply->saddr));

            inet_ntop(AF_INET, &g_ft_traceroute.sa4.sin_addr, ip_str, sizeof(ip_str));
            ret = strcmp(inet_ntoa(*(struct in_addr *)&ip_reply->saddr), ip_str) == 0;
        }
        printf(" %sms ", recv_time);
    }

    fflush(stdout);
    return ret;
}

void traceroute_loop(void) {
    char    ip_str[INET_ADDRSTRLEN];
    uint8_t is_curr_hop_printed = 0, is_ip_printed = 0, is_dest_found = 0;
    struct timeval  *tv = &(struct timeval){0, 0};

    fill_data();

    init_icmp_socket();
    init_udp_socket();

    inet_ntop(AF_INET, &g_ft_traceroute.sa4.sin_addr, ip_str, sizeof(ip_str));
    printf("%s to %s (%s), %d hops max\n", PROG_NAME, g_ft_traceroute.host_arg_name, ip_str, g_ft_traceroute.options.max_hop);

    for (int curr_hop = 1; curr_hop <= g_ft_traceroute.options.max_hop; ++curr_hop) {
        g_ft_traceroute.sa4.sin_port = htons(g_ft_traceroute.options.port++);

        is_curr_hop_printed = 0;
        is_ip_printed = 0;

        for (int curr_try = 0; curr_try < g_ft_traceroute.options.tries; ++curr_try) {
            gettimeofday(tv, NULL);
            ssize_t bytes_sent = sendto(g_ft_traceroute.udp_sockfd, g_ft_traceroute.packet, 9,
                0, (struct sockaddr *)&g_ft_traceroute.sa4, sizeof(g_ft_traceroute.sa4));

            if (!is_curr_hop_printed) {
                is_curr_hop_printed = 1;
                printf("%3d  ", curr_hop);
            }

            if (bytes_sent < 0) {
                fflush(stdout);
                fprintf(stderr, "%s: sendto: %s\n", PROG_NAME, strerror(errno));
                exit_traceroute(EXIT_FAILURE);
            }

            if (receive_packet(tv, &is_ip_printed)) {
                is_dest_found = 1;
            }
        }

        printf("\n");
        fflush(stdout);

        set_socket_ttl(++g_ft_traceroute.options.ttl);

        if (is_dest_found) {
            break ;
        }
    }
}