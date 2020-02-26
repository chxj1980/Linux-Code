#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>

int main() {
	int res = alarm(1);
	int cnt = 0;
	while (1) {
		printf("cnt = %d\n", cnt ++);
	}
	return 0;
}
