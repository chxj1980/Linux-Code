#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("eg: ./a.out port\n");
    exit(1);
  }
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(fd != -1);

  // 连接服务器
  struct sockaddr_in ser;
  bzero(&ser, sizeof(ser));
  ser.sin_family = AF_INET;
  int port = atoi(argv[1]);
  ser.sin_port = htons(port);
  int res = inet_pton(AF_INET, "127.0.0.1", &(ser.sin_addr.s_addr));
  assert(res != -1);

  res = connect(fd, (const struct sockaddr*)&ser, sizeof(ser));
  assert(res != -1);
  while (1) {
    // 发送数据
    char buf[128] = {0};
    printf("please input: ");
    fgets(buf, sizeof(buf), stdin);
    if (strncmp("end", buf, 3) == 0 ) break;
		write(fd, buf, strlen(buf));

    // 等待接受数据
    int n = read(fd, buf, sizeof(buf));
    if (n == -1) {
      perror("read error!");
      exit(1);
    } else if (n == 0) {
      printf("server disconnected!\n");
      break;
    } else {
      printf("recv buf: %s\n", buf);
    }
  }
  close(fd);
  return 0;
}
