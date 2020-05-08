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



void send_fd(int fd, int cfd) {
  struct iovec vec;
  struct msghdr msg;
  char senddata;

  vec.iov_base = &senddata;
  vec.iov_len = 1;

  msg.msg_name = nullptr;
  msg.msg_namelen = 0;
  msg.msg_flags = 0;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;

  cmsghdr cm;
  cm.cmsg_len = CMSG_LEN(sizeof(int));
  cm.cmsg_level = SOL_SOCKET;
  cm.cmsg_type = SCM_RIGHTS;

  *(int*)CMSG_DATA(&cm) = cfd;
  msg.msg_control = &cm;
  msg.msg_controllen = CMSG_LEN(sizeof(int));
  sendmsg(fd, &msg, 0);
}


int recv_fd(int fd) {
  struct iovec vec;
  struct msghdr msg;
  char recvdata;
  vec.iov_base = &recvdata;
  vec.iov_len = 1;
  msg.msg_name = nullptr;
  msg.msg_namelen = 0;
  msg.msg_flags = 0;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;

  cmsghdr cm;
  msg.msg_control = &cm;
  msg.msg_controllen = CMSG_LEN(sizeof(int));

  recvmsg(fd, &msg, 0);
  return *(int*)CMSG_DATA(&cm);
}


int main() {
  int fds[2];
  if (socketpair(PF_UNIX, SOCK_STREAM, 0, fds) < 0) {
    perror("socketpair");
    exit(1);
  }
  pid_t pid = fork();
  if(pid == -1) {
    perror("fork");
    exit(1);
  }
  if (!pid) {
    close(fds[0]);
    int fd = open("in.txt", O_RDONLY);
    assert(fd != -1);
    send_fd(fds[1], fd);
  } else {
    close(fds[1]);
    int fd = recv_fd(fds[0]);
    char buffer[64] = {0};
    read(fd, buffer, sizeof(buffer));
    printf("recv data: %s\n", buffer);
  } 

  return 0;  
}