#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main() {
	struct itimerval it;
	
	// 第一次触发的时间
	it.it_value.tv_sec = 1;
	it.it_value.tv_usec = 0;

	// 设置触发的周期
	it.it_interval.tv_sec = 1;
	it.it_interval.tv_usec = 0;

	// 设置定时器
	int res = setitimer(ITIMER_REAL, &it, NULL);
	assert(res != -1);

	while (1) {
		printf("hello world!\n");
	}
	return 0;
}
