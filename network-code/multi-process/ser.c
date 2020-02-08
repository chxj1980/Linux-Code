#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

/*
 *  1.共享
 *    读时共享，写时复制
 *    文件描述符
 *    内存映射区 mmap
 *    
 *  2.父进程的角色
 *    等待接受客户端的连接 accept
 *    有连接时 accept
 *      创建一个子进程
 *      将通信的文件描述符关闭
 *  3.子进程的角色
 *    通信
 *      使用accept的返回值
 *    关闭监听的文件描述符
 *      浪费资源
 *  4.创建的进程的个数的限制
 *    (1)受到硬件限制
 *    (2)文件描述符的上线也是1024
 *  5.子进程资源回收
 *    wait/waitpid
 *    使用信号回收
 *    捕捉信号  SIGCHLD
 *
 *    信号的两种 signal sigaction
 * */


void recycle(int n) {
  pid_t pid;
  while (pid = waitpid(-1, NULL, WNOHANG) > 0) {
    printf("child died, pid = %d\n", pid);
  }
}

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
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
    perror("bind  error!");
    close(fd);
    exit(1);
 }

  res = listen(fd, 10);
  if (-1 == res) {
    perror("listen error!");
    close(fd);
    exit(1);
  }
  // 使用信号回收子进程的PCB
  struct sigaction act;
  act.sa_handler = recycle;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);
  sigaction(SIGCHLD, &act, NULL);
  struct sockaddr_in cli;
  socklen_t len = sizeof(cli);

  while (1) {
    // accept阻塞的时候被信号中断，处理信号对应的操作之后
    // 回来之后就不阻塞了，这时候errno == EINTR
    int cfd = accept(fd, (struct sockaddr*)&cli, &len);
    while  (-1 == cfd && errno == EINTR) {
      cfd = accept(fd, (struct sockaddr*)&cli, &len);
    }
    printf("connect successful!\n");
    pid_t pid = fork();
    if (-1 == pid) {
      perror("fork error!");
      close(fd);
      close(cfd);
      exit(1);
    } else if (0 == pid) {
      printf("my id is %d\n", getpid());
      close(fd); // 关闭监听的文件描述符
      char ip[64] = {0};
      while (1) {
        printf("client ip: %s, port: %d\n",
            inet_ntop(AF_INET, &cli.sin_addr.s_addr, ip, sizeof(ip)), ntohs(cli.sin_port));
        char buf[128] = {0};
        int n = read(cfd, buf, sizeof(buf));
        if (n == -1) {
          perror("read error!");
          exit(1);
        } else if (n == 0) {
          printf("client disconnected!\n");
          close(cfd);
          break;
        } else {
          printf("client: %s\n", buf);
          write(cfd, buf, n);
        }
      }
      // 不通信时，退出子进程 
      return 0;
    } else if (pid > 0) {
      close(cfd); // 关闭通信的文件描述符
      // 父进程需要做资源回收
      
    }
  }
  return 0;
}
