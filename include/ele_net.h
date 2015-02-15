#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

in_addr_t ele_net_get_ipv4addr_from(
	const char * const if_name);

#ifdef __cplusplus
}
#endif
