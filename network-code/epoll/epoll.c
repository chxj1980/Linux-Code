#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



#define MAXEPOLLEVENT 2000



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
	
	// 创建epoll树的根节点
	int epfd = epoll_create(2000);
	if (-1 == epfd) {
		perror("epoll_create:");
		exit(0);
	}
	
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = lfd;
	res = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	if (-1 == res) {
		perror("epoll_ctl:");
		exit(0);
	}

	struct epoll_event events[MAXEPOLLEVENT];
	while (1) {
		// 使用epoll通知内核
		int n = epoll_wait(epfd, events, MAXEPOLLEVENT, -1);
		for (int i=0; i<n; ++i) {
			int fd = events[i].data.fd;
			if (lfd == fd) {
				// 接受连接请求
				struct sockaddr_in cli;
				socklen_t len = sizeof(cli);
				int cfd = accept(fd, (struct sockaddr*)&cli, &len);
				if (-1 == cfd) {
					perror("accept error:");
					exit(0);
				}
				struct epoll_event cev;
				cev.events = EPOLLIN;
				cev.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &cev); 
				char ip[64] = {0};
				printf("ip:%s, port:%d\n",
						inet_ntop(AF_INET, &cli.sin_addr.s_addr, ip
							,sizeof(ip)), ntohs(cli.sin_port));
			} else {
				// 处理已经来连接的客户端
				if (!events[i].events & EPOLLIN) continue;
				char buf[128] = {0};
				int len = recv(fd, buf, 128, 0);
				if (-1 == len) {
					perror("recv error:");
					exit(0);
				} else if (0 == len) {
					printf("client disconnected!\n");
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
					close(fd); //close需要再摘节点之后
				} else {
					printf("buf: %s\n", buf);
					send(fd, "OK", 2, 0);
				}
			}

		}
	}

	return 0;
}
