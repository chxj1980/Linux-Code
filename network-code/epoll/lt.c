#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*
水平触发：
只要fd对应的缓冲区有数据
epoll_wait就得返回  触发的次数与发送数据的次数没有关系


*/

void add(int epfd, int fd) {
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	int res = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	assert(res != -1);
}

void del(int epfd, int fd) {
	int res = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
	assert(res != -1);
}

int main(int argc, char *argv[]) {
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == lfd) {
		perror("socket error");
		exit(0);
	}
	struct sockaddr_in ser;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(argv[1]));
	ser.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = bind(lfd, (struct sockaddr*)&ser, sizeof(ser));
	if (-1 == res) {
		perror("bind error");
		exit(0);
	}

	res = listen(lfd, 5);
	if (-1 == res) {
		perror("listen error");
		exit(0);
	}

	int epfd = epoll_create(2000);
	assert(epfd != -1);
	
	add(epfd, lfd);
	
	struct epoll_event events[2000];
	
	int cnt = 0;
	while (1) {
		int n = epoll_wait(epfd, events, 2000, -1);
		printf("epoll_wait()\n"); cnt ++;
		assert(n != -1);
		for (int i=0; i<n; ++i) {
			int fd = events[i].data.fd;
			if (fd == lfd) {
				struct sockaddr_in cli;
				socklen_t len = sizeof(cli);
				int cfd = accept(fd, (struct sockaddr*)&cli, &len);
				assert(cfd != -1);
				add(epfd, cfd);
			} else {
				if (!events[i].events & EPOLLIN) continue;
				char buf[128] = {0};
				int n = recv(fd, buf, 1, 0);
				if (-1 == n) {
					perror("recv error:");
					close(fd);
					exit(0);
				} else if (0 == n) {
					printf("client %d diconnected!\n", fd);
					del(epfd, fd);
					close(fd);
				} else {
					printf("buf: %s\n", buf);
					send(fd, "OK", 2, 0);
				}
			}
		}
		printf("cnt = %d\n", cnt);
	}
	return 0;
}
