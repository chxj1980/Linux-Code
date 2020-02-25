#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (-1 == fd) {
    perror("socket error!");
    exit(1);
  }
  struct sockaddr_in ser;
  bzero(&ser, sizeof(ser));

  ser.sin_family = AF_INET;
  ser.sin_port = htons(8000);
  ser.sin_addr.s_addr = inet_addr("127.0.0.1");

  while (1) {
    printf("please input: ");
    char buf[128] = {0};
    fgets(buf, sizeof(buf), stdin);
    if (0 == strncmp(buf, "end", 3)) break;
    int res = sendto(fd, buf, strlen(buf)-1, 0, (struct sockaddr*)& ser, sizeof(ser));
    if (-1 == res) {
      perror("sendto error!");
      break;
    }
    char buf1[128] = {0};
    int n = recvfrom(fd, buf1, 127, 0, NULL, NULL);
    if (n <= 0) {
      perror("recvfrom error!");
      break;
    }
    printf("recvfrom: %s\n", buf1);
  }
  close(fd);
}
