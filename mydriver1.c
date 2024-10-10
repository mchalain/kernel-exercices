/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/poll.h>

#include <linux/gpio.h>

#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/of.h>

#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

MODULE_DESCRIPTION("retrovision gpio");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

int debounce = 20;
module_param(debounce, int, 0644);

#define CHANGED 0x80
struct mydriver1_data_t
{
	char name[11];
	struct miscdevice misc;
	struct gpio_desc *gpio;
	struct list_head list;
	struct tasklet_struct tasklet;
	struct delayed_work work;
	wait_queue_head_t wq;
	int state;
};

/*
 * Arguments
 */

static LIST_HEAD(mydevice_list);


/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)file->private_data;
	int length = 0;
	int ret;

	if ((file->f_flags & O_NONBLOCK) == 0)
	{
		ret = wait_event_interruptible(ddata->wq, ddata->state);
		if (ret < 0) {
		  printk(KERN_DEBUG "testintr: woke up by signal\n");
		  return -ERESTARTSYS;
		}
	}
	ret = gpiod_get_raw_value(ddata->gpio);
	if (count > 0)
	{
		if (ret < 10)
			buf[0] = 0x30 + ret;
		else if (ret < 0)
			buf[0] = 'F';
		buf[1] = 0;
		length = 2;
	}
	count = length;
	*ppos += count;
	ddata->state &= CHANGED;
	ddata->state = 0;
	return count;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	*ppos += count;
	
	return count;
}

static unsigned int my_poll(struct file *file, poll_table *wait)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)file->private_data;
	unsigned int mask = 0;

	poll_wait(file, &ddata->wq,  wait);
	mask |= POLLIN | POLLRDNORM;
    return mask;
}

static int my_open(struct inode *inode, struct file *file)
{
	struct mydriver1_data_t *ddata = NULL, *pos;
	list_for_each_entry(pos, &mydevice_list, list)
	{
		if (pos->misc.minor == iminor(inode))
		{
			ddata = pos;
			break;
		}
	}
	if (ddata)
	{
		ddata->state = 0;
		file->private_data = ddata;
		return 0;
	}
	return EBUSY;
}

static int my_release(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations my_fops = {
	.owner =	THIS_MODULE,
	.read =		my_read,
	.write =	my_write,
	.poll = 	my_poll,
	.open =		my_open,
	.release =	my_release,
};

static void my_check(unsigned long data)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)data;
	/* wake up */
	ddata->state = gpiod_get_value(ddata->gpio);
	ddata->state |= CHANGED;
	wake_up_interruptible (&ddata->wq);
}

static void my_debounce_work(struct work_struct *work)
{
	struct mydriver1_data_t *ddata =
		container_of(work, struct mydriver1_data_t, work.work);

	my_check((unsigned long)ddata);
}

static irqreturn_t my_irq_handler(int irq, void * data)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)data;
	/* Schedule the tasklet */
	if(ddata->tasklet.func)
		tasklet_schedule(&ddata->tasklet);
	else
		mod_delayed_work(system_wq, &ddata->work, msecs_to_jiffies(debounce));

	return IRQ_HANDLED;
}

static int my_probe(struct platform_device *dev)
{
	const char *string = NULL;
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)dev_get_platdata(&dev->dev);

	of_property_read_string(dev->dev.of_node, "devname", &string);
	if (string)
	{
		if (ddata == NULL)
		{
			pr_info("gpio create data\n");
			ddata = devm_kzalloc(&dev->dev, sizeof(*ddata), GFP_KERNEL);
			snprintf(ddata->name, sizeof(ddata->name), string);
			ddata->gpio = devm_gpiod_get(&dev->dev, string, GPIOD_OUT_HIGH);
			if (ddata->gpio > 0)
			{
				gpiod_direction_input(ddata->gpio);
			}
		}
	}
	if (ddata)
	{
		int irq = -1;
		irq = gpiod_to_irq(ddata->gpio);
		if (irq > 0)
			irq = devm_request_irq(&dev->dev, irq, my_irq_handler, IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, THIS_MODULE->name, ddata);

		if (!irq)
		{
			if (debounce && gpiod_set_debounce(ddata->gpio, debounce * 1000))
			{
				pr_info("gpio soft debounce\n");
				ddata->tasklet.func = NULL;
				INIT_DELAYED_WORK(&ddata->work, my_debounce_work);
			}
			else
			{
				tasklet_init(&ddata->tasklet, my_check, (unsigned long)ddata);
			}
			init_waitqueue_head(&ddata->wq);
		}
		else
		{
			ddata = NULL;
		}
	}
	if (ddata)
	{
		struct miscdevice *misc;

		misc = &ddata->misc;
		misc->minor = 110 + desc_to_gpio(ddata->gpio);
		misc->name = ddata->name;
		misc->fops = &my_fops;

		misc_register(misc);

		list_add(&ddata->list, &mydevice_list);
		return 0;
	}
	return -1;
}

static int my_remove(struct platform_device *dev)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)dev->dev.platform_data;
	if (ddata)
	{
		int irq = -1;
		list_del(&ddata->list);
		misc_deregister(&ddata->misc);
		irq = gpiod_to_irq(ddata->gpio);
		devm_free_irq(&dev->dev, irq, ddata);
		devm_gpiod_put(&dev->dev, ddata->gpio);
	}
	return 0;
}

struct of_device_id my_devices_table[] =
{
	{
		.compatible = "mydriver1",
	},
	{
		.compatible = "example,mydriver1",
	},
	{
		.compatible = "test,mydriver1",
	},
	{}
};

static struct platform_driver my_driver =
{
	.driver = {
		.name = "mydriver1",
		.of_match_table = my_devices_table
	},
	.probe = my_probe,
	.remove = my_remove,
};


static int __init my_init(void)
{

	platform_driver_register(&my_driver);
	return 0;
}

static void __exit my_exit(void)
{
	platform_driver_unregister(&my_driver);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
