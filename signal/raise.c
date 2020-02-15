#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>


int main() {
	pid_t pid = fork();
	assert(pid != -1);
	if (0 == pid){
		// kill(getpid(), SIGKILL);
		raise(SIGINT); // 自己给自己发信号
	}	else {
		int stat;
		pid_t res = wait(&stat);
		assert(res != -1);
		printf("child died, pid: %d\t\n", res);
		if (WIFSIGNALED(stat)) {
			printf("died by signal , %d\t\n", WTERMSIG(stat));
		}
	}
	return 0;
}
