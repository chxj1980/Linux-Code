#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <assert.h>
#define PAGESIZE 4096


int main() {
  int shmid = shmget((key_t)1234, PAGESIZE, 0664|IPC_CREAT);
  assert(shmid != -1);

  char *p = (char*)shmat(shmid, NULL, 0);
  assert(p != (char*)-1);
  while (1) {
    if (strncmp(p, "end", 3) == 0) break;
    printf("len = %d, %s\n", strlen(p)-1, p);
    sleep(5);
  }
  shmdt(p);
  return 0;
}
