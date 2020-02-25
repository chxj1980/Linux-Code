#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 
int select(int nfds, fd_set *readfds, fd_set *writefds,
fd_set* exceptfds, struct timeval *timeout);


nfds: 要检测的文件描述符最大的fd+1 1024
readfds:读集合 
writefds:写集合
exceptfds:异常集合
timeout:
 NULL 永久阻塞
	当检测到文件描述有变化时就返回
 定时
 	struct timeval a;
	a.tv_sec = 0
	a.tv_usec = 0 
	非阻塞



void FD_ZERO(fd_set *set);
void FD_CLR(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
int FD_ISSET(int fd, fd_set *set);

优点：跨平台
缺点：
1.每次调用select，都需要把fd从用户太拷贝到内核态
这个开销在fd很多的时候会很大
2.同时每次调用select都需要在内核遍历传递进来的所有fd,
这个开销在fd很多的时候会也很大
3.select支持的文件描述符数量太小了，默认是1024
*/





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
	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);
	
	int maxfd = sockfd;
	// 文件描述符集合
	fd_set rfds, tmp;
	FD_ZERO(&rfds);
	FD_ZERO(&tmp);
	FD_SET(sockfd, &rfds);
	while (1) {

		// 委托内核去检测 
		tmp = rfds;
		int n = select(maxfd+1, &tmp, NULL, NULL, NULL);
		if (-1 == n) {
			perror("select error:");
			exit(0);
		}
		// 1.有客户端发起了新连接
		if (FD_ISSET(sockfd, &tmp)) {
			int cfd = accept(sockfd, (struct sockaddr*)&cli, &len);
			if (-1 == cfd) {
				perror("accept error:");
				exit(0);
			}
			char ip[64] = {0};
			printf("ip: %s, port : %d\n", 
					inet_ntop(AF_INET, &cli.sin_addr.s_addr, ip, sizeof(ip)), ntohs(cli.sin_port));
			// 将cfd加入待检测的集合中 -- 下一次就可以检测到了
			FD_SET(cfd, &rfds);
			// 更新值最大的文件描述符
			maxfd = maxfd < cfd ? cfd : maxfd;
		}

		for (int i=sockfd+1; i<=maxfd; ++i) {
			if (FD_ISSET(i, &tmp)) {
				char buf[1024] = {0};
				int len = recv(i, buf, sizeof(buf), 0);
				if (len == -1) {
					perror("recv error:");
					exit(0);
				} else if (len == 0) {
					printf("client disconnected!\n");
					close(i);
					FD_CLR(i, &rfds);
				} else {
					printf("%s\n", buf);
					send(i, buf, strlen(buf), 0);
				}
			}	
		}
	}
	return 0;
}
