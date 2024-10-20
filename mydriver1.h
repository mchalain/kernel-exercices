#ifndef __MYDRVIER1_H__
#define __MYDRVIER1_H__

struct mydriver1_data_t
{
	char name[11];
	struct miscdevice *misc;
	struct platform_device *pdev;
	int read;
	struct list_head list;
	struct gpio_desc *gpio;
	int irq;
	int count;
	int state;
	struct tasklet_struct tasklet;
	wait_queue_head_t wq;
};

#endif
