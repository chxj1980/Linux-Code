#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>

/*
 int sigaction(int signum, const struct sigaction* act, struct sigaction* oldact);

 struct sigaction{
 	void      (*sa_handler)(int);
	void      (*sa_sigaction)(int, siginfo_t*, void*);
	sigset_t  sa_mask; // 在信号处理函数执行过程中临时屏蔽指定的信号
	int 			sa_flags;

	sa_handler flag 0
	sa_sigaction flag 其他
	void (*sa_restore)(void); //已经被废弃
 };
 */

void fun(int x) {
	printf("signal number: %d\n", x);
	printf("hello world\n");
	sleep(3);
	printf("wake up!\n");
}

int main() {
	struct sigaction sig;
	sig.sa_handler = fun;
	sig.sa_flags = 0;
	sigaddset(&sig.sa_mask, SIGQUIT);
	// sigemptyset(&sig.sa_mask);
	sigaction(SIGQUIT, &sig, NULL);

	while (1);
	return 0;
}
