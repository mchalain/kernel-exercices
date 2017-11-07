#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "mydriver1.h"

int main(int argc, char **argv)
{
	char *device = NULL;
	int gpio_nr = 0;

	int opt;
	do
	{
		opt = getopt(argc, argv, "hd:n:");
		switch (opt)
		{
			case 'h':
				printf("%s -d <device path> -n <gpio number>", argv[0]);
				exit(-1);
			break;
			case 'd':
				device = optarg;
			break;
			case 'n':
				gpio_nr = atoi(optarg);
			break;
		}
	} while(opt != -1);

	if (device == NULL)
		return -1;
	int fd = open(device, O_RDWR);
	if (fd > 0)
	{
		int ret;
		int run = 1;
		while (run)
		{
			int c = getchar();
			switch (c)
			{
				case 'q':
					run = 0;
				break;
				case '1':
					ret = ioctl(fd, RPI_GPIO_SET, (void *)gpio_nr);
				break;
				case '0':
					ret = ioctl(fd, RPI_GPIO_CLEAR, (void *)gpio_nr);
				break;
				case 'r':
				{
					uint32_t value;
					ret = ioctl(fd, RPI_GPIO_GET, &value);
					printf("gpio %d value %ld\n", gpio_nr, value);
				}
				break;
			}
		}
		close(fd);
	}
	else
	{
		fprintf(stderr, "open error : %s\n", strerror(errno));
	}
	return 0;
}
