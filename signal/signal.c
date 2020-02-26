#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

void fun(int x) {
	printf("signal number: %d\t\n", x);
	printf("ctrl + \\ \n");
}

int main() {
	while (1) {
		signal(SIGINT, fun);
	}
	return 0;
}
