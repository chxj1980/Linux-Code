#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>



typedef struct msgdata{
  long mtype;
  char mtext[128];
}MSG;

int main() {
  int id = msgget((key_t)1234, 0664|IPC_CREAT);
  assert(id != -1);

  MSG msg;
  memset(&msg, 0, sizeof(msg));

  msgrcv(id, &msg, 127, 200, 0);
  printf("msg.mtype: %ld\n", msg.mtype);
  printf("msg.mtext: %s\n", msg.mtext);
  return 0;
}
