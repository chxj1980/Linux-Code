#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

/*
三个函数
epoll_wait();
epoll_ctl();
epoll_create();

int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);
op:
EPOLL_CTL_ADD
EPOLL_CTL_DEL
EPOLL_CTL_MOD

*/






#define MAX_EVENT_NUMBER 1024
#define BUF_SIZE 10

#define bool int
#define true 1
#define false 0


int setnoblocking(int fd) {
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}


void add(int epfd, int fd, bool et) {
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if (et) ev.events |= EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	setnoblocking(fd);
}


void LT(struct epoll_event* events, int number, int epfd, int lfd) {
	char buf[BUF_SIZE];
	for (int i=0; i<number; ++i) {
		int sockfd = events[i].data.fd;
		if (sockfd == lfd) {
			struct sockaddr_in cli;
			socklen_t len = sizeof(cli);
			int cfd = accept(lfd, (struct sockaddr*)&cli, &len);
			if (cfd == -1) {
				perror("LT error");
				break;
			}
			add(epfd, cfd, false);
		} else if (events[i].events & EPOLLIN) {
			printf("event trigger once\n");
			memset(buf, '\0', sizeof(buf));
			int n = recv(sockfd, buf, BUF_SIZE-1, 0);
			if (n <= 0) {
				close(sockfd)
				continue;
			}
			printf("get %d byte of content: %s\n", n, buf);
		} else {
			printf("something else happended!\n");
		}
	}
}


void ET(struct epoll_event* events, int number, int epfd, int lfd) {
	char buf[BUF_SIZE];
	for (int i=0; i<number; ++i) {
		int sockfd = events[i].data.fd;
		if (sockfd == lfd) {
			struct sockaddr_in cli;
			socklen_t len = sizeof(cli);
			int cfd = accept(lfd, (struct sockaddr*)&cli, &len);
			if (-1 == cfd) {
				peror("accept error:");
				break;
			}
			add(epfd, cfd, true);
		} else if (events[i].data & EPOLLIN) {
			printf("event trigger once!\n");
			while (1) {
				memset(buf, '\0', sizeof(buf));
				int n = recv(sockfd, buf, BUF_SIZE-1, 0);
				if (n < 0) {
					if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
						printf("read later!\n");
						break;
					}
					close(sockfd);
					break;
				} else if (0 == n) {
					close(sockfd);
				} else {
					printf("get %d bytes of content: %s\n", n, buf);
				}
			}
		} else {
			printf("something else happended!\n");
		}
	}
}



int main() {
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

	int flag = atoi(argv[2]);
	struct epoll_event events[MAX_EVENT_NUMBER];
	int epfd = epoll_create(5);
	assert(epfd != -1);
	add(epfd, sockfd, true);
	while (1) {
		int ret = epoll_wait(epfd, events, MAX_EVENT_NUMBER, -1);
		if (ret < 0) {
			printf("epoll failure!\n");
			break;
		}	
		if (flag) {
			ET(events, ret, epfd, sockfd);
		} else {
			LT(events, ret, epfd, sockfd);
		}
	}
	close(sockfd);
	return 0;
}
