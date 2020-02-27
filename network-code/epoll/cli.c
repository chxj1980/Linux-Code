#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, const char *argv[]) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("socket error");
		exit(0);
	}
	struct sockaddr_in ser;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(argv[1]));
	ser.sin_addr.s_addr = inet_addr("127.0.0.1");
	int res = connect(sockfd, (struct sockaddr*)&ser, sizeof(ser));
	if (-1 == res) {
		perror("connect error:");
		exit(0);
	}

	while (1) {
		printf("please input:");
		char buf[128] = {0};
		fgets(buf, 128, stdin);
		if (!strncmp(buf, "bye", 3)) {
			close(sockfd);
			break;
		}
		send(sockfd, buf, strlen(buf)-1, 0);
		char msg[128] = {0};
		int n = recv(sockfd, msg, 127, 0);
		if (n <= 0) {
			perror("recv error:");
			close(sockfd);
			break;
		}
		printf("len = %d\n", n);
		printf("%s\n", buf);
	}
	return 0;
}
