#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
using namespace std;


int main() {
  int fd = open("/dev/tty", O_RDONLY);
  if (isatty(fd)) { 
    cout << "this is a tty" << endl;
    cout << "ttyname = " << ttyname(fd) << endl;
  } else {
    cout << "this is not a tty" << endl;
  }
  close(fd);
  return 0;
}
