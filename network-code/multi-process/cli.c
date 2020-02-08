#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("eg: ./a.out port\n");
    exit(1);
  }
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == fd) {
    perror("socket error!");
    exit(1);
  }
  struct sockaddr_in ser;
  ser.sin_family = AF_INET;
  int port = atoi(argv[1]);
  ser.sin_port = htons(port);
  ser.sin_addr.s_addr = inet_addr("127.0.0.1");

  int res = connect(fd, (struct sockaddr*)&ser, sizeof(ser));
  if (-1 == res) {
    perror("connect error!");
    exit(1);
  }
  printf("connect server successful!\n");
  while (1) {
    printf("please input: ");
    char buf[1024] = {0};
    fgets(buf, 1023, stdin);
    if (0 == strncmp(buf, "end", 3)) break;
    send(fd, buf, strlen(buf)-1, 0);

    char msg[1024] = {0};
    int n = recv(fd, msg, 1023, 0);
    if (n <= 0) {
      perror("recv error!");
      break;
    }
    printf("ser: %s\n", msg);
  }
  close(fd);
  return 0;
}
