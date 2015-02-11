#define _GNU_SOURCE // secure_getenv
#include <stdlib.h>

#include "ele_meas.h"

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "ele_error.h"
#include "ele_net.h"

#ifndef ELE_MES_REPORT_DEST_IPV4
#define ELE_MES_REPORT_DEST_IPV4 "ELE_MES_REPORT_DEST_IPV4"
#endif

#ifndef ELE_MES_REPORT_DEST_PORT
#define ELE_MES_REPORT_DEST_PORT "ELE_MES_REPORT_DEST_PORT"
#endif

//#ifndef ELE_MES_REPORT_DEVICE
//#define ELE_MES_REPORT_DEVICE "ELE_MES_REPORT_DEVICE"
//#endif

static const int WELL_KNOWN_PORT_MAX = 1023;

static int sock = -1;
//static struct sockaddr_in multicast_addr;
static struct sockaddr_in addr;

__attribute__((constructor))
ele_result_t
ele_meas_init(void)
{
	const char * const ipv4_name = getenv(ELE_MES_REPORT_DEST_IPV4);
	if (ipv4_name == NULL) {
		perror("getenv(ELE_MES_REPORT_DEST_IPV4)");
		return ELE_FAILURE;
	}

	const char * const port_name = getenv(ELE_MES_REPORT_DEST_PORT);
	if (port_name == NULL) {
		perror("getenv(ELE_MES_REPORT_DEST_PORT)");
		return ELE_FAILURE;
	}
	if (strlen(port_name) <= 0) {
		perror("ELE_MES_REPORT_DEST_PORT: invalid length");
	}
	int port = (int)strtol(port_name, (char**)NULL, 10);
	if (port == LONG_MAX || port < WELL_KNOWN_PORT_MAX) {
		perror("ELE_MES_REPORT_DEST_PORT: invalid value");
		return -1;
	}

	int s = sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1) {
		perror("socket");
		return ELE_FAILURE;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ipv4_name);

//	const char * const ipv4_name = getenv(ELE_MES_REPORT_DEST_IPV4);
//	if (ipv4_name != NULL) {
//		multicast_addr.sin_family = AF_INET;
//		multicast_addr.sin_port = htons(12345);
//		multicast_addr.sin_addr.s_addr = inet_addr(ipv4_name);
//	} else {
//		perror("secure_getenv(ELE_MES_REPORT_DEST_IPV4)");
//		return ELE_FAILURE;
//	}
//
//	const char * const device_name = getenv(ELE_MES_REPORT_DEVICE);
//	if (device_name != NULL) {
//		int s = socket(AF_INET, SOCK_DGRAM, 0);
//		if (s == -1) {
//			perror("socket");
//			return ELE_FAILURE;
//		}
//
//		char loopch=0;
//		if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP,
//					   (char *)&loopch, sizeof(loopch)) != 0) {
//			perror("setsockopt/IP_MULTICAST_LOOP");nd
//			close(s);
//			return ELE_FAILURE;
//		}
//
//		in_addr_t ipaddr = ele_net_get_ipv4addr_from(device_name);
//		if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF,
//			(char *)&ipaddr, sizeof(ipaddr)) != 0) {
//			perror("setsockopt/IP_MULTICAST_IF");
//			close(s);
//			return ELE_FAILURE;
//		}
//
//		int ttl = 10;
//		if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL,
//			(char *)&ttl, sizeof(ttl)) != 0) {
//			perror("setsockopt/IP_MULTICAST_TTL");
//			close(s);
//			return ELE_FAILURE;
//		}
//
//		sock = s;
//		return ELE_SUCCESS;
//
//	} else {
//		perror("secure_getenv(ELE_MES_REPORT_DEVICE)");
//		return ELE_FAILURE;
//	}
//	return ELE_FAILURE;
}

void ele_meas_clock(
	uint32_t attr[ELE_MES_DATA_BLOCKS])
{
	if (sock == -1) return;

	struct meas_data_packet {
		uint64_t tv_sec;
		uint64_t tv_nsec;
		uint32_t attr[ELE_MES_DATA_BLOCKS];
	} packet;

	struct timespec tp;
	int clock_gettime_result = clock_gettime(CLOCK_REALTIME, &tp);
	if (clock_gettime_result == 0) {
		packet.tv_sec = tp.tv_sec;
		packet.tv_nsec = tp.tv_nsec;
		memcpy(packet.attr, attr, sizeof(packet.attr));
		int flags = MSG_DONTWAIT;
		int result = sendto(sock, &packet, sizeof(packet), flags,
			(struct sockaddr *)&addr, sizeof(addr));
			//(struct sockaddr *)&multicast_addr, sizeof(multicast_addr));
		if (result == -1) {
			perror("sendto");
		}
	} else {
		perror("clock_gettime");
	}
}
