#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/msg.h>
#include <assert.h>
#include <string.h>

int main() {
  int msgid = msgget((key_t)1234, 0664|IPC_CREAT);
  assert(msgid != -1);
  struct msqid_ds msg;
  int res = msgctl(msgid, IPC_STAT, &msg);
  assert(res != -1);
  printf("msg.key: %d\n", msg.msg_perm.__key);
  printf("msg.uid: %d\n", msg.msg_perm.uid);
  printf("msg.gid: %d\n", msg.msg_perm.gid);
  printf("msg.cuid: %d\n", msg.msg_perm.cuid);
  printf("msg.cgid: %d\n", msg.msg_perm.cgid);
  printf("msg.mode: %d\n", msg.msg_perm.mode);
  printf("msg.seq: %d\n", msg.msg_perm.__seq);
  res = msgctl(msgid, IPC_RMID, NULL);
  assert(res != -1);
  system("ipcs -q");
  return 0;
}
