#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

in_addr_t ele_net_get_ipv4addr_from(
	const char * const if_name);
