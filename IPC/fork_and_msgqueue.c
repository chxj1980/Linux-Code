#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <assert.h>


typedef struct msgbuf {
  long mtype;
  char mtext[128];
} MSG;


int main() {
  int msgid = msgget((key_t)1235, 0664|IPC_CREAT);
  assert(msgid != -1);
  pid_t pid = fork();
  assert(pid != -1);
  if (pid > 0) {
    MSG msg;
    memset(&msg, 0, sizeof(msg));
    msg.mtype = 100;
    strcpy(msg.mtext, "hello");
    msgsnd(msgid, &msg, strlen(msg.mtext), 0);
  } else if (pid == 0) {
    MSG msg;
    memset(&msg, 0, sizeof(msg));
    msgrcv(msgid, &msg, 127, 100, 0);
    printf("msg.mtype: %ld\n", msg.mtype);
    printf("msg.mtext: %s\n", msg.mtext);
  }
  return 0;
}
