#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdlib.h>


int main() {
	pid_t pid = fork();
	assert(pid != -1);
	if (pid > 0) {
		int stat;
		pid_t res = wait(&stat);
		assert(res != -1);
		printf("child pid: %d\t\n", res);
		if (WIFSIGNALED(stat)) {
			printf("died by signale: %d\t\n", WTERMSIG(stat));
		}
	} else {
		while (1) {
			abort();
		}
	}
	return 0;
}
