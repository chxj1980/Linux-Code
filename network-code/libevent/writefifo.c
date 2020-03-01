#include <stdio.h>
#include <fcntl.h>
#include <event2/event.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>


void write_cb(int fd, short what, void* arg) {
	// 写管道
	char buf[1024] = {0};
	static int cnt = 0;
	sprintf(buf, "hello world == %d\n", cnt ++);
	write(fd, buf, strlen(buf));
}


int main(int argc, const char* argv[]) {

	int fd = open("myfifo", O_WRONLY|O_NONBLOCK);
	if (-1 == fd) {
		perror("open error");
		exit(0);
	}

	struct event_base* base = event_base_new();
	
	// 检测的是写缓冲区是否有空间去写
	struct event* ev = event_new(base, fd, EV_WRITE|EV_PERSIST, write_cb, NULL);
	event_add(ev, NULL);

	event_base_dispatch(base);
	event_free(ev);

	event_base_free(base);
	close(fd);
	return 0;
}
