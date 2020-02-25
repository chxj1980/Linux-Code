#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>


/*
int poll(struct pollfd* fds, nfds_t nfds, int timeout);
struct pollfd {
	int fd; //文件描述符
	short events; // 等待的事件
	short revents; //实际发生的事件
}

nfds:有效元素的最后一个的下标+1
timeout
-1 永久阻塞
 0 调用完立即返回
>0 等待的时长毫秒 

返回值:IO发送变化的文件描述符的个数
读事件
写事件
错误事件
*/
#define LEN 100
// 初始化struct pollfd数组，
void init(struct pollfd* fds) {
	for (int i=0; i<LEN; ++i) {
		fds[i].fd = -1;
	}
	return;
}

// 从struct pollfd数组中删除fd
void del(struct pollfd* fds, int fd) {
	for (int i=0; i<LEN; ++i) {
		if (fds[i].fd == fd) {
			fds[i].fd = -1;
			break;
		}
	}
	return;
}

void add(struct pollfd* fds, int fd) {
	for (int i=0; i<LEN; ++i) {
		if (fds[i].fd == -1) {
			fds[i].fd = fd;
			fds[i].events = POLLIN | POLLRDHUP;
			break;
		}
	}
}

void getClient(struct pollfd* fds, int fd) {
	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);
	int cfd = accept(fd, (struct sockaddr*)&cli, &len);
	if (cfd == -1) {
		perror("accept error:");
		exit(0);
	}
	printf("client: %d connected!\n", cfd);
	char ip[64] = {0}; 
	printf("ip = %s, port = %d\n", inet_ntop(AF_INET, &cli.sin_addr.s_addr, ip, sizeof(ip)), ntohs(cli.sin_port));
	add(fds, cfd);
}


// POLLRDHUP TCP连接被对方关闭，或者对方关闭了写操作
void handlerClientData(struct pollfd* fds,int fd,  int rdhup) {
	printf("handlerClientData()\n");
	if (rdhup) {
		close(fd);
		del(fds, fd);
		return;
	}
	char buf[128] = {0};
	int n = recv(fd, buf, 127, 0);
	printf("n = %d\n", n);
	if (-1 == n) {
		perror("recv error:");
		exit(0);
	} else if (0 == n) {
		printf("client disconnected!\n");
		close(fd);
		return;
	} else {
		printf("buf: %s\n", buf);
		send(fd, "OK", 2, 0);
	}
}




int main(int argc, char *argv[]) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("socket error");
		exit(0);
	}
	struct sockaddr_in ser;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(argv[1]));
	ser.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = bind(sockfd, (struct sockaddr*)&ser, sizeof(ser));
	if (-1 == res) {
		perror("bind error");
		exit(0);
	}

	res = listen(sockfd, 5);
	if (-1 == res) {
		perror("listen error");
		exit(0);
	}
	
	struct pollfd fds[LEN];
	init(fds);
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;

	while (1) {
		int n = poll(fds, LEN, -1);
		if (n <= 0) {
			perror("poll error:");
			exit(0);
		}

		for (int i=0; i<LEN; ++i) {
			if (fds[i].fd == -1) continue;
			if (fds[i].fd == sockfd && (fds[i].revents & POLLIN)) {
				getClient(fds, sockfd);
			} else if (fds[i].revents & POLLIN) {
				if (fds[i].revents & POLLRDHUP) {
					handlerClientData(fds, fds[i].fd, 1);
				} else {
					handlerClientData(fds, fds[i].fd, 0);
				}
			}
		}
	}
	return 0;
}
