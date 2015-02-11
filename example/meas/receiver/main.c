#define _GNU_SOURCE // secure_getenv
#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdint.h>

int main(int argc, char * argv[]) {
	if (argc != 3) {
		fprintf(stderr, "udpr [ip] [port]\n");
		return -1;
	}

	int sock;
	struct sockaddr_in addr;

	struct meas_data_packet {
		uint64_t tv_sec;
		uint64_t tv_nsec;
		uint32_t attr[4];
	};

	printf("ip=%s, port=%s\n", argv[1], argv[2]);
	int port = strtol(argv[2], (char**) NULL, 10);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	bind(sock, (struct sockaddr *) &addr, sizeof(addr));

	char buff[1024];

	const struct meas_data_packet * const buf = (struct meas_data_packet*) buff;

	int i = 0;
	while (1) {
		int siz = recv(sock, buff, sizeof(buff), 0);
		if (siz <= 0)
			break;

		if ((i++ % 100) < 10) {

			printf("%lld.%09lld %x.%x.%x.%x\n", buf->tv_sec, buf->tv_nsec,
					buf->attr[0], buf->attr[1], buf->attr[2], buf->attr[3]);
		}
	}

	close(sock);

	return 0;
}
