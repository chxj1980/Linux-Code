#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

// 定义
typedef struct SockInfo {
  int fd;
  struct sockaddr_in cli;
  pthread_t id;
} sockinfo;


void* worker(void *arg) {
  sockinfo* info = (sockinfo*)arg;
  char buf[1024] = {0};
  char ip[64] = {0};
  printf("my thread id is: %ld\n", info->id);

  // 通信
  while (1) {
    printf("client ip: %s, port: %d\n",
        inet_ntop(AF_INET, &info->cli.sin_addr.s_addr, ip, sizeof(ip)
          ), ntohs(info->cli.sin_port));
    int n = read(info->fd, buf, sizeof(buf));
    if (-1 == n) {
      perror("read error!");
      pthread_exit(NULL); //退出线程
    } else if (0 == n) {
      printf("client disconnected!\n");
      close(info->fd);
      break;
    } else {
      printf("recv: %s\n", buf);
      write(info->fd, buf, n);
    }
  }
  return NULL;
}


int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == fd) {
    perror("socket error!");
    exit(0);
  }

  struct sockaddr_in ser;
  ser.sin_family = AF_INET;
  ser.sin_port = htons(8888);
  ser.sin_addr.s_addr = inet_addr("127.0.0.1");

  int res = bind(fd, (struct sockaddr*)&ser, sizeof(ser));
  if (-1 == res) {
    perror("bind error!");
    exit(0);
  }

  res = listen(fd, 5);
  if (-1 == res) {
    perror("listen error!");
    exit(0);
  }

  sockinfo info[256];
  for (unsigned int i=0; i<sizeof(info)/sizeof(info[0]); ++i) {
    info[i].fd = -1;
  }

  socklen_t len = sizeof(struct sockaddr_in);
  int i = 0;
  while (1) {
    // 选择一个没有被使用最小下标的数组元素
    // 主线程等待连接请求
    // int cfd; 这样是错误的
    for (i=0; i<sizeof(info)/sizeof(info[0]); ++i) {
      if (info[i].id == -1) {
        break;
      }
    }
    info[i].fd = accept(fd, (struct sockaddr*)&info[i].cli, &len);
    // 创建子线程 -- 通信
    pthread_create(&info[i].id, NULL, worker, &info[i]);
    // 直接进行线程分离
    pthread_detach(info[i].id);
  }
  close(fd);
  // 只退出主线程, 对子线程没有影响
  pthread_exit(NULL);


  return 0;
}
