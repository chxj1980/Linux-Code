#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == fd) {
    perror("socket error!");
    exit(1);
  }

  struct sockaddr_in ser;
  bzero(&ser, sizeof(ser));
  ser.sin_family = AF_INET;
  ser.sin_port = htons(8888);
  ser.sin_addr.s_addr = htonl(INADDR_ANY);

  int res = bind(fd, (struct sockaddr*)&ser, sizeof(ser));
  if (-1 == res) {
    perror("bind error!");
    exit(1);
  }

  // 监听
  res = listen(fd, 5);
  if (-1 == res) {
    perror("listen error!");
    exit(1);
  }

  struct sockaddr_in cli;
  socklen_t len = sizeof(cli);
  int cfd = accept(fd, (struct sockaddr*) &cli, &len);
  if (-1 == cfd) {
    perror("accept error!");
    exit(1);
  }
  printf("accept successful!\n");
  char ipbuf[64] = {0};
  printf("client ip and port: %s, %d\n",
      inet_ntop(AF_INET, &cli.sin_addr.s_addr,ipbuf, sizeof(ipbuf)), ntohs(cli.sin_port));
  while (1) {
    char buf[1024] = {0};
    int n = read(cfd, buf, sizeof(buf));
    if (n == -1) {
      perror("read error!");
      close(cfd);
      break;
    } else if (n == 0) {
      printf("cli disconnected!\n");
      close(cfd);
      break;
    } else {
      printf("recv buf: %s\n", buf);
      for (int i=0; i<n; ++i) {
        buf[i] = toupper(buf[i]);
      }
      printf("send buf: %s\n", buf);
      write(cfd, buf, strlen(buf));
    }
  }
  close(fd);
  return 0;
}
