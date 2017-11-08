#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include "mydriver1.h"

int main(int argc, char **argv)
{
	char *device = NULL;
	char *gpio_nr = NULL;

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
				gpio_nr = optarg;
			break;
		}
	} while(opt != -1);

	if (device == NULL)
		return -1;
	if (gpio_nr != NULL)
	{
		int fd = open("/sys/module/mydriver1/parameters/gpio_nr", O_RDWR);
		if (fd > 0)
		{
			write(fd, gpio_nr, 2);
			close(fd);
		}
	}
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
					ret = ioctl(fd, RPI_GPIO_SET);
				break;
				case '0':
					ret = ioctl(fd, RPI_GPIO_CLEAR);
				break;
				case 'r':
				{
					uint32_t value;
					ret = ioctl(fd, RPI_GPIO_GET, &value);
					printf("gpio value %ld\n", value);
				}
				break;
			}
		}
		close(fd);
	}
	return 0;
}
