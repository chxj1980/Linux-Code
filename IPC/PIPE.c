#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


int main() {
  int fds[2] = {-1, -1};
  int res = pipe(fds);
  assert(res != -1);
  //  printf("%d %d\n", fds[0], fds[1]);
  pid_t id = fork();
  assert(id != -1);
  if (id == 0) {
    close(fds[1]);// 关闭写端
    while (1) {
      char buff[128] = {0};
      int n = read(fds[0], buff, 127);
      if (n <= 0 || 0 == strncmp(buff, "end", 3)) break;
      printf("child: %s\n", buff);
    }
    close(fds[0]);
  } else {
    close(fds[0]);
    while (1) {
      printf("please input: ");
      char buff[128] = {0};
      fgets(buff, 128, stdin);
      write(fds[1], buff, strlen(buff)-1);
      if (strncmp(buff, "end", 3) == 0) break;
    }
    close(fds[1]);
  }
  return 0;
}
