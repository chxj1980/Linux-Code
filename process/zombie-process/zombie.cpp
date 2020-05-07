#include <unistd.h>
#include <iostream>
using namespace std;

int main() {
  pid_t pid = fork();
  if (-1 == pid) {
    perror("fork");
    exit(0);
  }
  if (!pid) {
    cout << "child process" << endl;
    cout << "pid : " << getpid() << ", ppid : " << getppid() << endl; 
  } else {
    sleep(2);
    cout << "parent process" << endl;
    cout << "pid : " << getpid() << ", ppid : " << getppid() << endl;
    for(;;) {}
  }
  return 0;
}