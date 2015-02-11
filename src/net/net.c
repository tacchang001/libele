#include "ele_net.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

in_addr_t ele_net_get_ipv4addr_from(
	const char * const if_name)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		perror("socket");
		close(fd);
		return -1;
	}

	struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, if_name, IFNAMSIZ-1);
	int result = ioctl(fd, SIOCGIFADDR, &ifr);
	if (result == -1) {
		perror("ioctl(SIOCGIFCONF)");
		close(fd);
		return -1;
	}

	close(fd);

	return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
}
