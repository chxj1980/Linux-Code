#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main() {
	// 手动屏蔽信号
	sigset_t myset;
	sigemptyset(&myset);// 清空
	sigaddset(&myset, SIGINT);
	sigaddset(&myset, SIGQUIT);
	sigaddset(&myset, SIGKILL);

	// 自定义信号集设置给内核阻塞信号集
	sigprocmask(SIG_BLOCK, &myset, NULL);

	while (1) {
		sigset_t set;
		sigpending(&set);
		for (int i=1; i<32; ++i) {
			if (sigismember(&set, i)) {
				printf("1");
			} else {
				printf("0");
			}
		}
		printf("\n");
	}
	return 0;
}
