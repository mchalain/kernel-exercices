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

/*
 * Arguments
 */
static short int my_major = 0;

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
static struct class *my_class = NULL;

static int __init my_init(void)
{
	int ret;
	struct device *device = NULL;
	dev_t dev = 0;

	my_class = class_create(THIS_MODULE, "mydrivers");

	ret = alloc_chrdev_region(&dev, 0, 1, "mydriver");
	if (ret < 0) panic("Couldn't register /dev/tty driver\n");

	my_major = MAJOR(dev);

	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;

	ret = cdev_add(&my_cdev, MKDEV(my_major, 0), 1);
	if (ret) panic("Couldn't register /dev/mydriver driver\n"); 


	device = device_create(my_class, NULL, MKDEV(my_major, 0), NULL, "mydriver");
	
	return 0;
}

static void __exit my_exit(void)
{
	device_destroy(my_class, MKDEV(my_major, 0));
	cdev_del(&my_cdev);
	class_destroy(my_class);
	unregister_chrdev_region(MKDEV(my_major, 0), 1);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
