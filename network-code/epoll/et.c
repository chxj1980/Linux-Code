#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
客户端给server发数据：
发送一次数据 server epoll_wait返回一次
剩余的数据在缓冲区里边

不在乎数据是否会读完
*/

void add(int epfd, int fd) {
	struct epoll_event ev;
	ev.events = EPOLLIN|EPOLLET;
	ev.data.fd = fd;
	int res = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	assert(res != -1);
}

void del(int epfd, int fd) {
	int res = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
	assert(res != -1);
}



int main(int argc, const char* argv[]) {
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
	
	struct epoll_event events[2000];
	int epfd = epoll_create(2000);
	assert(epfd != -1);
	add(epfd, lfd);
	while (1) {
		int ret = epoll_wait(epfd, events, 2000, -1);
		printf("epoll_wait\n");
		assert(ret != -1);
		for (int i=0; i<ret; ++i) {
			int fd = events[i].data.fd;
			if (fd == lfd) {
				struct sockaddr_in cli;
				socklen_t len = sizeof(cli);
				int cfd = accept(fd, (struct sockaddr*)&cli, &len);
				assert(cfd != -1);
				add(epfd, cfd);
			} else {
				if (events[i].events & EPOLLIN) {
					char buf[128] = {0};
					int n = recv(fd, buf, 1, 0);	
					assert(n != -1);
					if (n == 0) {
						printf("client %d diconnected!\n", fd);
						del(epfd, fd);
						close(fd);
					} else {
						printf("buf: %s\n", buf);
						send(fd, "OK", 2, 0);
					}
				}
			}
		}
	}

	return 0;
}
