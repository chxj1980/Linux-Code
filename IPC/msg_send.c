#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/msg.h>

typedef struct msgdata{
  long mtype; //消息类型
  char mtext[128];
} MSG;


int main() {
  int id = msgget((key_t)1234, IPC_CREAT | 0664);
  assert(id != -1);

  MSG msg;
  memset(&msg, 0, sizeof(msg));
  msg.mtype = 100;
  strcpy(msg.mtext, "hello world");
  msgsnd(id, &msg, strlen(msg.mtext), 0);
 
  memset(&msg, 0, sizeof(msg));
  msg.mtype = 200;
  strcpy(msg.mtext, "ni hao, shi jian");
  msgsnd(id, &msg, strlen(msg.mtext), 0);
}
