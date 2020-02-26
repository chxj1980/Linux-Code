#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int res = alarm(5);
	printf("res = %d\t\n", res);
	sleep(2);
	res = alarm(2);
	printf("res = %d\t\n", res);

	while (1) {
		printf("hell world!\n");
		sleep(1);
	}
	return 0;
}
