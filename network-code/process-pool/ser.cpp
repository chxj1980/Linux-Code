#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <signal.h>
#include <fcntl.h>

static const int CONTROL_LEN = CMSG_LEN(sizeof(int));
int createSocket() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(-1 != fd);
  struct sockaddr_in ser;
  ser.sin_family = AF_INET;
  ser.sin_port = htons(7000);
  ser.sin_addr.s_addr = inet_addr("127.0.0.1");
  int res = bind(fd, (struct sockaddr*)&ser, sizeof(ser));
  assert(res != -1);
  listen(fd, 5);
  return fd;
}
int recvClinetFd(int fd) {
  struct iovec iov[1];
	struct msghdr msg;
	char buf[0];

	iov[0].iov_base = buf;
	iov[0].iov_len = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	cmsghdr cm;
	msg.msg_control = &cm;
	msg.msg_controllen = CONTROL_LEN;

	recvmsg(fd, &msg, 0);

	int c = *(int *)CMSG_DATA(&cm);

	return c;
}
void sendClinetFd(int cfd, int fd) {
  struct iovec iov[1];
	struct msghdr msg;
	char buf[0];

	iov[0].iov_base = buf;
	iov[0].iov_len = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

  cmsghdr cm;
	cm.cmsg_len = CONTROL_LEN;
	cm.cmsg_level = SOL_SOCKET;
	cm.cmsg_type = SCM_RIGHTS;

	*(int *)CMSG_DATA(&cm) = cfd;
	msg.msg_control = &cm;
	msg.msg_controllen = CONTROL_LEN;

	sendmsg(fd, &msg, 0);
}

void workProcess(int fd) {
  while (1) {
    int cfd = recvClinetFd(fd);
    while (1) {
      char buffer[128] = {0};
      int n = recv(cfd, buffer, 127, 0);
      if (n <= 0) {
        close(cfd);
        break;
      }
      printf("recv: %s\n", buffer);
      send(cfd, "ok", 2, 0);
    }
  }
}


pid_t child[128];
int cnt = 0;
void signalHandler(int sig) {
  for (int i=0; i<cnt; ++i) {
    kill(child[i], SIGKILL);
  }
}

int main() {
  signal(SIGINT, signalHandler);
  int sockfd = createSocket();

  int pipefd[2];
  int res = socketpair(PF_UNIX, SOCK_DGRAM, 0, pipefd);
  for (int i=0; i<3; ++i) {
    pid_t pid = fork();
    assert(-1 != pid);
    child[cnt ++] = pid;
    if (!pid) {
      close(pipefd[1]);
      workProcess(pipefd[0]);
      exit(0);
    }
  }
  close(pipefd[0]);
  while(1) {
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);
    int cfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    assert(-1 != cfd);
    sendClinetFd(cfd, pipefd[1]);
    close(cfd);
  }
  return 0;
}