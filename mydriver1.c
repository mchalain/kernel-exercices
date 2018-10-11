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
#define MY_FIRSTMINOR 0
static short int my_major = 0;
static short int my_minor_range = 3;
module_param(my_minor_range, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(my_minor_range, "The range of minor device number");

static char *my_data = "my char driver\n";
/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	// ppos may contain the position inside the driver's buffer.
	if (count > 16)
		strncpy(buf, my_data + *ppos, 16 - *ppos);
	// the returned count has to be the number of bytes copied inside the user buffer.
	count = 16 - *ppos;
	*ppos += count;
	// while count is up to 0 the user application may continue to read.
	// a normal application should close the file on a 0 returned.
	printk(KERN_INFO "my char driver: read() returns %lu\n", count);
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
	int i;

	// generate dynamic MAJOR MINOR
	ret = alloc_chrdev_region(&dev, MY_FIRSTMINOR, my_minor_range, "mydrivers");
	if (ret < 0) panic("Couldn't register /dev/tty driver\n");

	my_major = MAJOR(dev);

	// create link between MAJOR/MINOR and file operations
	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;
	ret = cdev_add(&my_cdev, dev, my_minor_range);
	if (ret < 0) panic("Couldn't register /dev/mydriver driver\n");

	// create entry "mydrivers/" into /sys/class/
	my_class = class_create(THIS_MODULE, "mydrivers");
	for (i = 0; i < my_minor_range; i++)
	{
		dev_t devno = MKDEV(my_major, MY_FIRSTMINOR + i);
		// create entries "mydriver[i]/" and "mydriver[i]/dev" into /sys/class/mydrivers/
		device = device_create(my_class, NULL, devno, NULL, "mydriver%d", i);
	}

	return 0;
}

static void __exit my_exit(void)
{
	int i;
	for (i = 0; i < my_minor_range; i++)
	{
		dev_t devno = MKDEV(my_major, MY_FIRSTMINOR + i);

		device_destroy(my_class, devno);
	}
	cdev_del(&my_cdev);
	class_destroy(my_class);
	unregister_chrdev_region(MKDEV(my_major, MY_FIRSTMINOR), my_minor_range);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
