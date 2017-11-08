/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#include "myclass.h"
#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controler */

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) 
// or SET_GPIO_ALT(x,y)
#define INP_GPIO(addr,g) *((addr)+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(addr,g) *((addr)+((g)/10)) |=  (1<<(((g)%10)*3))

#define GPIO_SET(gpio) *((gpio)+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR(gpio) *((gpio)+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET(gpio) *((gpio)+13)
// For GPIO# >= 32 (RPi B+)
#define GPIO_SET_EXT(gpio) *(gpio+8)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR_EXT(gpio) *(gpio+11) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET_EXT(gpio) *((gpio)+14)

/*
 * Arguments
 */
static short int my_minor = 0;

unsigned long *my_virtaddr;

static int gpio_nr = 16;
module_param(gpio_nr, int, 0644);
static int gpio_dir = 0;
module_param(gpio_dir, int, 0644);

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
		if (gpio_nr >= 32)
			GPIO_SET_EXT(my_virtaddr) = (1 << (gpio_nr % 32));
		else
			GPIO_SET(my_virtaddr) = (1 << gpio_nr);
	}
	else if (cmd == RPI_GPIO_CLEAR)
	{
		if (gpio_nr >= 32)
			GPIO_CLR_EXT(my_virtaddr) = (1 << (gpio_nr % 32));
		else
			GPIO_CLR(my_virtaddr) = (1 << gpio_nr);
	}
	else if (cmd == RPI_GPIO_GET)
	{
		unsigned value = 0;
		if (gpio_nr >= 32)
			value = GPIO_GET_EXT(my_virtaddr);
		else
			value = GPIO_GET(my_virtaddr);
		pr_info("value %u\n", value);
		*(uint32_t *)arg = !!(value & (1 << gpio_nr));
		pr_info("arg %u\n", *(uint32_t *)arg);
	}
	return 0;
}

static int my_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "my char driver: open()\n");

	if (gpio_dir)
		INP_GPIO(my_virtaddr, gpio_nr);
	else
		OUT_GPIO(my_virtaddr, gpio_nr);
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
	if ((my_virtaddr = ioremap (GPIO_BASE, PAGE_SIZE)) == NULL) {
		printk(KERN_ERR "Can't map GPIO addr !\n");
		return -1;
	}
	return 0;
}

static void __exit my_exit(void)
{
	myclass_unregister(my_minor);
	iounmap (my_virtaddr);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
