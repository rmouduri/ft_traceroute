#ifndef _FT_TRACEROUTE_H_
#define _FT_TRACEROUTE_H_

#include <limits.h>
#include <netinet/in.h>

#define PROG_NAME	"ft_traceroute"

#define TRACEROUTE_HELP	"Usage: "PROG_NAME" [OPTION...] HOST\n" \
						"Print the route packets trace to network host.\n" \
						"\n" \
						"  -f, --first-hops=NUM      set initial hop distance, i.e., time-to-live\n" \
						"  -m, --max-hop=NUM         set maximal hop count (default: 64)\n" \
						"  -p, --port=PORT           use destination PORT port (default: 33434)\n" \
						"  -q, --tries=NUM           send NUM probe packets per hop (default: 3)\n" \
						"  -w, --wait=NUM            wait NUM seconds for response (default: 3)\n" \
						"  -?, --help                give this help list\n" \
						"      --usage               give a short usage message\n"

#define TRACEROUTE_USAGE	"Usage: "PROG_NAME" [-?] [-f NUM] [-m NUM] [-p PORT]\n" \
							"               [-q NUM] [-w NUM] [--first-hop=NUM]\n" \
							"               [--max-hop=NUM] [--port=PORT]\n" \
							"               [--tries=NUM] [--wait=NUM]\n" \
							"               [--help] [--usage] HOST"

#define TRACEROUTE_ERROR_USAGE	"Try '"PROG_NAME" --help' or '"PROG_NAME" --usage' for more information."
#define TRACEROUTE_MISSING_HOST	PROG_NAME": missing host operand"
#define TRACEROUTE_UNKNOWN_HOST	PROG_NAME": unknown host"
#define TRACEROUTE_IPV6_SUPPORT	PROG_NAME": ipv6 not supported"


#define MIN_FIRST_HOP	1
#define MAX_FIRST_HOP	UCHAR_MAX
#define MIN_HOPS		1
#define MAX_HOPS		UCHAR_MAX
#define MIN_TRIES		1
#define MAX_TRIES		10
#define MIN_WAIT		0
#define MAX_WAIT		60
#define MIN_PORT		1
#define MAX_PORT		USHRT_MAX + 1


#define DEFAULT_FIRST_HOP	1
#define DEFAULT_MAX_HOP		64
#define DEFAULT_TRIES		3
#define DEFAULT_WAIT		3
#define DEFAULT_PORT		33434


typedef enum e_options_flag {
	FIRST_HOP	=	1 << 0,
	MAX_HOP		= 	1 << 1,
	TRIES		=	1 << 2,
	WAIT		=	1 << 3,
	PORT		=	1 << 4
} t_options_flag;

typedef enum e_address_type {
    NONE,
    IPV4,
    IPV6,
    FQDN
} address_type;

typedef struct s_options {
	t_options_flag	flags;

	uint16_t	ttl;
	uint8_t		max_hop;
	uint8_t		tries;
	uint8_t		wait;
	uint16_t	port;
} t_options;


typedef struct s_ft_traceroute {
	t_options		options;

	char *			host_arg_name;
	int				udp_sockfd;
	int				icmp_sockfd;
	address_type	address_type;

    struct addrinfo		*res;
	struct sockaddr_in	sa4;

	uint8_t			packet[USHRT_MAX];
	uint8_t			buffer[USHRT_MAX];
} t_ft_traceroute;



extern t_ft_traceroute g_ft_traceroute;


// options.c
void get_options(int argc, char **argv);
void print_ft_traceroute_global(void);


// traceroute.c
// void init_socket(void);
void traceroute_loop(void);


// error.c
void exit_traceroute(int ret);
void exit_missing_host(int ret);
void exit_error_usage(int ret);
void exit_help(int ret);
void exit_usage(int ret);
void exit_ipv6_support(int ret);
void exit_unknown_host(int ret);

// socket.c
void set_socket_ttl(uint8_t ttl);
void init_udp_socket(void);
void init_icmp_socket(void);


#endif //_FT_TRACEROUTE_H_