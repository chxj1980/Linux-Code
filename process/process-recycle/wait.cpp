#include <unistd.h>
#include <iostream>
#include <wait.h>
using namespace std;


int main() {
  pid_t pid = fork();
  if (-1 == pid) {
    perror("fork");
    exit(-1);
  }

  if (!pid) {
    cout << "child process" << " pid = " << getpid() << ", ppid = " << getppid() << endl;
    sleep(2);
    while(1) {
      sleep(2);
      cout << "child process running" << endl;
    }
  } else {
    cout << "parent process" << " pid = " << getpid() << ", ppid = " << getppid() << endl;
    // 回收子进程的资源
    // pid_t childpid = wait(NULL); // NULL对退出状态不关系
    // cout << "my child pid = " << childpid << endl;
    int status;
    pid_t childpid = wait(&status);
    cout << "my child pid = " << childpid << endl;
    if (WIFEXITED(status)) {
      cout << "exit state : " << WEXITSTATUS(status) << endl;
    } else if (WIFSIGNALED(status)) {
      cout << "signal number : " << WTERMSIG(status) << endl;
    }
  }
  return 9;
}