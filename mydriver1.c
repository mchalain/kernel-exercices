/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/miscdevice.h>

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

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

static struct miscdevice mymisc;

static int __init my_init(void)
{
	mymisc.minor = MISC_DYNAMIC_MINOR;
	mymisc.name = "mydriver";
	mymisc.fops = &my_fops;

	misc_register(&mymisc);
	return 0;
}

static void __exit my_exit(void)
{
	misc_deregister(&mymisc);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
