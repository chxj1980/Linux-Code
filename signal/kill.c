#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main() {
	pid_t pid = fork();
	assert(pid != -1);
	if (0 == pid) {
		sleep(2);
		kill(getppid(), SIGKILL);
		exit(0);
	}	else {
		while (1) {
			printf("father process, pid:%d\t\n", getpid());
			sleep(1);
		}
	}
	return 0;
}
