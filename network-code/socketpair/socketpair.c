#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>

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
    int x;
    while (1) {
      read(fds[1], &x, sizeof(x));
      ++ x;
      write(fds[1], &x, sizeof(x));
    }
  } else {
    int x = 0;
    close(fds[1]);
    while (1) {
      ++ x;
      printf("parent process: %d, send data: %d\n", getpid(), x);
      write(fds[0], &x, sizeof(x));
      read(fds[0], &x, sizeof(x));
      printf("parent process: %d, recv data: %d\n", getpid(), x);
      sleep(1);
    }
  }

  return 0;  
}
