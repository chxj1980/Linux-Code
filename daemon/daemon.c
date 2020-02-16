#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


void work(int x) {
	// 当前的系统时间写入磁盘
	time_t res;
	time(&res);
	// 格式化时间
	char* t = ctime(&res);
	// 写入文件
	int fd = open("/home/lsc/tmp/time.txt", O_CREAT | O_WRONLY | O_APPEND, 0664);
	assert (fd != -1);
	write(fd, t, strlen(t)+1);
	close(fd);
}



int main() {
	pid_t pid = fork();
	assert(pid != -1);
	if (pid == 0) {
		int res = setsid();
		assert(res != -1);
	
	
		const char *path = "/home/lsc/tmp";
		res = chdir(path);
		assert(res != -1);
		
		umask(0);
		close(0);
		close(1);
		close(2);
		
		struct sigaction act;
		act.sa_handler = work;
		act.sa_flags = 0;
		sigemptyset(&act.sa_mask);
		sigaction(SIGALRM, &act, NULL);

		struct itimerval tv;
		tv.it_interval.tv_sec = 1;
		tv.it_interval.tv_usec = 0;
		tv.it_value.tv_sec = 2;
		tv.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &tv, NULL);
		while (1);
	} else {
		sleep(2);
		kill(getpid(), SIGKILL);
	}
	return 0;
}
