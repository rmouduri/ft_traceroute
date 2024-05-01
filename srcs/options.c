#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ft_traceroute.h"


void get_options(int argc, char **argv) {
    int opt;
    char *endptr;
    unsigned long n;
    struct option long_options[] = {
        {"first-hop", required_argument, 0, 'f'},
        {"max-hop", required_argument, 0, 'm'},
        {"port", required_argument, 0, 'p'},
        {"tries", required_argument, 0, 'q'},
        {"usage", no_argument, 0, 'u'},
        {"wait", required_argument, 0, 'w'},
        {"help", no_argument, 0, '?'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "f:m:p:q:w:?", long_options, NULL)) != -1) {
        switch(opt) {
            case '?':
                if (strcmp(argv[optind - 1], "-?") == 0 || strcmp(argv[optind - 1], "--help") == 0) {
                    exit_help(EXIT_SUCCESS);
                } else {
                    exit_error_usage(64);
                }
                break;
            case 'f':
                g_ft_traceroute.options.flags |= FIRST_HOP;
                n = strtoul(optarg, &endptr, 0);

                if (endptr == optarg || *endptr || n < MIN_FIRST_HOP || n > MAX_FIRST_HOP) {
                    fprintf(stderr, PROG_NAME": impossible distance `%s'\n", optarg);
                    exit_traceroute(EXIT_FAILURE);
                }
                memcpy(&g_ft_traceroute.options.ttl, &n, sizeof(g_ft_traceroute.options.ttl));
                break;
            case 'm':
                g_ft_traceroute.options.flags |= MAX_HOP;
                n = strtoul(optarg, &endptr, 0);

                if (endptr == optarg || *endptr || n < MIN_HOPS || n > MAX_HOPS) {
                    fprintf(stderr, PROG_NAME": invalid hops value `%s'\n", optarg);
                    exit_traceroute(EXIT_FAILURE);
                }
                memcpy(&g_ft_traceroute.options.max_hop, &n, sizeof(g_ft_traceroute.options.max_hop));
                break;
            case 'p':
                g_ft_traceroute.options.flags |= PORT;
                n = strtoul(optarg, &endptr, 0);

                if (endptr == optarg || *endptr || n < MIN_PORT || n > MAX_PORT) {
                    fprintf(stderr, PROG_NAME": invalid port number `%s'\n", optarg);
                    exit_traceroute(EXIT_FAILURE);
                }
                memcpy(&g_ft_traceroute.options.port, &n, sizeof(g_ft_traceroute.options.port));
                break;
            case 'q':
                g_ft_traceroute.options.flags |= TRIES;
                n = strtoul(optarg, &endptr, 0);

                if (endptr == optarg || *endptr) {
                    fprintf(stderr, "%s: invalid value (`%s' near `%s')\n", PROG_NAME, optarg, endptr);
                    exit_error_usage(EXIT_FAILURE);
                } else if (n < MIN_TRIES || n > MAX_TRIES) {
                    fprintf(stderr, PROG_NAME": number of tries should be between %d and %d\n", MIN_TRIES, MAX_TRIES);
                    exit_traceroute(EXIT_FAILURE);
                }
                memcpy(&g_ft_traceroute.options.tries, &n, sizeof(g_ft_traceroute.options.tries));
                break;
            case 'u':
                exit_usage(EXIT_SUCCESS);
                break;
            case 'w':
                g_ft_traceroute.options.flags |= WAIT;
                n = strtoul(optarg, &endptr, 0);

                if (endptr == optarg || *endptr || n > MAX_WAIT) {
                    fprintf(stderr, PROG_NAME": ridiculous waiting time `%s'\n", optarg);
                    exit_traceroute(EXIT_FAILURE);
                }
                memcpy(&g_ft_traceroute.options.wait, &n, sizeof(g_ft_traceroute.options.wait));
                break;
            default:
                exit_error_usage(EXIT_FAILURE);
                break;
        }
    }
}