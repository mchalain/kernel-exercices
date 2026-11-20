#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "mydriver1.h"

int main(int argc, char **argv)
{
	if (! argv[1])
		return 1;
	int fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	for (int i = 0; i < 5; i++) {
		ioctl(fd, RPI_GPIO_SET);
		sleep(1);
		ioctl(fd, RPI_GPIO_CLEAR);
		sleep(1);
	}

	close(fd);
	return 0;
}
