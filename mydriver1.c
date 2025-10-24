/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */
static struct miscdevice mymisc;

static int gpio_nr = 21;
module_param(gpio_nr, int, 0644);

static u32 clic = 0;
static irqreturn_t my_irq_handler(int irq, void * ident)
{
	clic ++;
	return IRQ_HANDLED;
}

/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	int i = 0;
	if (*ppos > 9)
		return 0;
	copy_to_user(buf, "nb click ", 9);
	count += 9;
	for (i = 0; i < 8; i++)
	{
		char c = (clic >> (i * 4)) & 0x0f;
		if (c < 0x0a)
			c += 0x30;
		else
			c += 0x40 - 0x0a;
		copy_to_user(buf + count, &c, 1);
		count += 1;
	}
	copy_to_user(buf + count, "\n", 1);
	count += 1;
	printk(KERN_INFO "gpio(%d) nb click %u\n", gpio_nr - 512, clic);
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
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
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
	int ret = 0;

	mymisc.minor = MISC_DYNAMIC_MINOR;
	mymisc.name = "mydriver";
	mymisc.fops = &my_fops;
	ret = misc_register(&mymisc);

	if (gpio_nr < 512)
		gpio_nr += 512;

	ret = gpio_request(gpio_nr, THIS_MODULE->name);
	if (ret)
		pr_info("gpio(%d) request error\n", gpio_nr);
	ret = gpio_direction_input(gpio_nr);
	if (ret)
		pr_info("gpio(%d) direction error\n", gpio_nr);
	ret = request_irq(gpio_to_irq(gpio_nr), my_irq_handler, IRQF_SHARED | IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name);
	if (ret)
		pr_info("gpio(%d) irq request error\n", gpio_nr);
	return 0;
}

static void __exit my_exit(void)
{
	free_irq(gpio_to_irq(gpio_nr), THIS_MODULE->name);
	gpio_free(gpio_nr);
	misc_deregister(&mymisc);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
