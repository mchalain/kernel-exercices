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

#include <linux/miscdevice.h>

#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */
static int gpio_nr = 21;
module_param(gpio_nr, int, 0644);

/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	int value = gpio_get_value(gpio_nr);
	printk(KERN_INFO"gpio(%d) value %d\n", gpio_nr,value);
	if (*ppos > 2)
		return 0;
	if (value)
	{
		copy_to_user(buf, "on\n", 3);
		count = 3;
	}
	else
	{
		copy_to_user(buf, "off\n", 4);
		count = 4;
	}
	*ppos += count;
	return count;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	count = 0;
	return count;
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
};
static struct miscdevice mymisc;

static int __init my_init(void)
{
	int ret = 0;
	mymisc.minor = MISC_DYNAMIC_MINOR;
	mymisc.name = "mydriver";
	mymisc.fops = &my_fops;
	ret = misc_register(&mymisc);
	if (ret)
		printk(KERN_INFO"misc register error\n");

	if (gpio_nr < 512)
		gpio_nr += 512;
	ret = gpio_request(gpio_nr, THIS_MODULE->name);
	if (ret)
		printk(KERN_INFO"gpio(%d) request error\n", gpio_nr);
	ret = gpio_direction_input(gpio_nr);
	if (ret)
		printk(KERN_INFO"gpio(%d) direction error\n", gpio_nr);
	return 0;
}

static void __exit my_exit(void)
{
	misc_deregister(&mymisc);
	gpio_free(gpio_nr);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
