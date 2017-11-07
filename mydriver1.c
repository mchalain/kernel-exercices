/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#include "myclass.h"
#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */
static short int my_minor = 0;

unsigned long *my_virtaddr;

static int gpio_nr = 21;

module_param(gpio_nr, int, 0644);

/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	count = 0;
	*ppos += count;
	return count;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	count = 0;
	return count;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	if (cmd == RPI_GPIO_SET)
	{
		short gpio = gpio_nr;
		if (arg)
			gpio = (short) arg;
		pr_info("set gpio %d\n", gpio);
		if (gpio_set_value(gpio, 1) == 0)
			gpio_nr = gpio;
	}
	else if (cmd == RPI_GPIO_CLEAR)
	{
		short gpio = gpio_nr;
		if (arg)
			gpio = (short) arg;
		pr_info("clear gpio %d\n", gpio);
		gpio_set_value(gpio, 0);
	}
	else if (cmd == RPI_GPIO_GET)
	{
		*(uint32_t *)arg = gpio_get_value(gpio_nr);
		pr_info("get gpio %d => %ld\n", gpio_nr, *(uint32_t *)arg);
	}
	return 0;
}

static int my_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "my char driver: open()\n");

	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "my char driver: release()\n");

	return 0;
}

static struct file_operations my_fops = {
	.owner =	THIS_MODULE,
	.read =		my_read,
	.write =	my_write,
	.open =		my_open,
	.release =	my_release,
	.unlocked_ioctl = my_ioctl,
};

static int __init my_init(void)
{
	my_minor = myclass_register(&my_fops, "mydriver", NULL);
	gpio_request(gpio_nr, THIS_MODULE->name);
	gpio_direction_output(gpio_nr,1);
	return 0;
}

static void __exit my_exit(void)
{
	myclass_unregister(my_minor);
	gpio_free(gpio_nr);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
