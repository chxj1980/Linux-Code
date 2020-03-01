#include <stdio.h>
#include <fcntl.h>
#include <event2/event.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


void read_cb(int fd, short what, void* arg) {
	// 读管道
	char buf[1024] = {0};
	int len = read(fd, buf, sizeof(buf));
	printf("data len:%d, buf data:%s\n", len, buf);
	printf("read event: %s", what&EV_READ ? "YES":"NO");
}


int main(int argc, const char* argv[]) {
	// 创建有名管道
	mkfifo("myfifo", 0664);

	int fd = open("myfifo", O_RDONLY|O_NONBLOCK);
	if (-1 == fd) {
		perror("open error");
		exit(0);
	}

	struct event_base* base = event_base_new();
	
	struct event* ev = event_new(base, fd, EV_READ|EV_PERSIST, read_cb, NULL);
	event_add(ev, NULL);

	event_base_dispatch(base);
	event_free(ev);

	event_base_free(base);
	close(fd);
	return 0;
}
