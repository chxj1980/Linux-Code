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

  char* ptr = shmat(shmid, NULL, 0);
  assert(ptr != (char*)-1);

  while (1) {
    printf("please input: ");
    fgets(ptr, 128, stdin);
    if (strncmp(ptr, "end", 3) == 0) break;
  }
  shmdt(ptr);
  return 0;
}
