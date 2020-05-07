#include <iostream>
#include <unistd.h>
using namespace std;


int main() {
  pid_t pid = fork();
  if (-1 == pid) {
    perror("fork");
    exit(1);
  }

  if (!pid) {
    sleep(1);
    cout << "========= child process =========" << endl;
    cout << "child:" << getpid() << "," << getppid() << endl;
  } else {
    cout << "========= parent process =========" << endl;
    cout << "parent:" << getpid() << "," << getppid() << endl;

  }
  return 0;
}