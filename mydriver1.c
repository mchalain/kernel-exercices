/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

#define MY_MAJOR 60
/*
 * Arguments
 */

/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "my char driver: read()\n");

	count = 0;
	*ppos += count;
	return count;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "my char driver: write()\n");

	*ppos += count;
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

static struct cdev my_cdev;

static int __init my_init(void)
{
	int ret;

	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;

	ret = cdev_add(&my_cdev, MKDEV(MY_MAJOR, 0), 1);
	if (ret) panic("Couldn't register /dev/mydriver driver\n"); 

	printk(KERN_INFO "my char driver: create special file\n");
	printk(KERN_INFO "\tmknod /dev/mydriver0 c 60 0\n");

	return 0;
}

static void __exit my_exit(void)
{
  cdev_del(&my_cdev);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
