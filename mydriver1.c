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

static int gpio_nr = 16;
module_param(gpio_nr, int, 0644);

/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	int value = gpio_get_value(gpio_nr);
	count = sprintf(buf, "%d\n", value);
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

static int __init my_init(void)
{
	my_minor = myclass_register(&my_fops, "mydriver", NULL);
	gpio_request(gpio_nr, THIS_MODULE->name);
	gpio_direction_input(gpio_nr);
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
