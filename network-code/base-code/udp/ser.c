#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (-1 == fd) {
    perror("socket error!");
    exit(1);
  }

  struct sockaddr_in ser;
  ser.sin_family = AF_INET;
  ser.sin_port = htons(8000);
  ser.sin_addr.s_addr = inet_addr("127.0.0.1");

  int res = bind(fd, (struct sockaddr*)&ser, sizeof(ser));
  if (-1 == res) {
    perror("bind error!");
    exit(1);
  }
  while (1) {
    char buf[128] = {0};
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);
    int n = recvfrom(fd, buf, 10, 0, (struct sockaddr*)&cli, &len);
    if (n <= 0) {
      perror("recvfrom error!");
      continue;
    }
    printf("client ip and port: %d, %s\n", ntohs(cli.sin_port), inet_ntoa(cli.sin_addr));
    printf("client: %s\n", buf);
    sendto(fd, "ok", 2, 0, (struct sockaddr*)&cli, sizeof(cli));
  }
  close(fd);
}
