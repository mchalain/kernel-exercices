/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>

#include "myclass.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */
static short int my_minor = 0;

unsigned char my_buffer[256];
unsigned int my_length = 0;
/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	int length = my_length;

	length = (length > count)? count: length;
	copy_to_user(buf, my_buffer + *ppos, length);
	my_length -= length;
	printk(KERN_INFO "my char driver: read(%s, %d)\n", my_buffer + *ppos, length);
	count = length;
	*ppos += count;
	return count;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	int length = 256;

	length -= *ppos;
	length = (length > count)? count: length;
	copy_from_user(my_buffer + *ppos, buf, length);
	my_length += length;
	printk(KERN_INFO "my char driver: write(%s,%d)\n", my_buffer, my_length);
	*ppos += count - my_length;
	if (*ppos > 256)
		*ppos %= 256;
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
	return 0;
}

static void __exit my_exit(void)
{
	myclass_unregister(my_minor);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
