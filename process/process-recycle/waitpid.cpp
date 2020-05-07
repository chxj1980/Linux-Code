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
  } else {
    cout << "parent process" << " pid = " << getpid() << ", ppid = " << getppid() << endl;
    int status;
    pid_t childpid;
    while ((childpid = waitpid(-1, &status, WNOHANG)) != -1) {
      if (!childpid) continue;
      cout << "my child pid = " << childpid << endl;
      
      if (WIFEXITED(status)) {
        cout << "exit state : " << WEXITSTATUS(status) << endl;
      } else if (WIFSIGNALED(status)) {
        cout << "signal number : " << WTERMSIG(status) << endl;
      }
    }
  }
  return 9;
}