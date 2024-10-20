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

#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/of.h>

#include <linux/gpio.h>
#include <linux/interrupt.h>	

#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */

static LIST_HEAD(mydevice_list);
static int mydevice_nb = 0;

/**
 * irq handler
 */
static irqreturn_t my_irq_handler(int irq, void * data)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)data;
	ddata->count++;
	return IRQ_HANDLED;
}

/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)file->private_data;
	struct platform_device *pdev = ddata->pdev;
	dev_info(&pdev->dev, "read()\n");
	size_t length = 4;
	int ret = 0;
	if (ddata->read >= length)
		return 0;

	ret = gpiod_get_raw_value(ddata->gpio);
	if (count > 4)
	{
		if (ret < 10)
			buf[0] = 0x30 + ret;
		else if (ret < 0)
			buf[0] = 'F';
		buf[1] = ':';
		buf[2] = 0x30 + (ddata->count & 0x09);
		buf[3] = 0;
		length = 4;
	}
	count = length;
	*ppos += count;
	ddata->read += count;
	return count;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)file->private_data;
	struct platform_device *pdev = ddata->pdev;
	dev_info(&pdev->dev, "write()\n");
	*ppos += count;
	return count;
}

static int my_open(struct inode *inode, struct file *file)
{
	struct mydriver1_data_t *ddata = NULL, *pos;
	list_for_each_entry(pos, &mydevice_list, list)
	{
		if (pos->misc->minor == iminor(inode))
		{
			ddata = pos;
			break;
		}
	}
	if (ddata)
	{
		struct platform_device *pdev = ddata->pdev;
		ddata->read = 0;
		file->private_data = ddata;
		dev_info(&pdev->dev, "open()\n");
		return 0;
	}
	return EBUSY;
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

static int my_probe(struct platform_device *pdev)
{
	const char * string = NULL;
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)dev_get_platdata(&pdev->dev);
	if (ddata == NULL)
	{
		ddata = devm_kzalloc(&pdev->dev, sizeof(*ddata), GFP_KERNEL);
		ddata->pdev = pdev;
	}
	dev_info(&pdev->dev, "probe ddata %p\n",ddata);
	if (ddata)
	{
		struct miscdevice *misc = devm_kmalloc(&pdev->dev, sizeof(*misc), GFP_KERNEL);
		snprintf(ddata->name, sizeof(ddata->name), "mydriver%d", mydevice_nb % 100);
		mydevice_nb++;
		misc->minor = MISC_DYNAMIC_MINOR;
		misc->name = ddata->name;
		misc->fops = &my_fops;

		misc_register(misc);
		dev_info(&pdev->dev, "misc minor %d\n", misc->minor);
		dev_set_drvdata(&pdev->dev, misc);
		ddata->misc = misc;
		list_add(&ddata->list, &mydevice_list);
	}
	of_property_read_string(pdev->dev.of_node, "devname", &string);
	if (string != NULL && ddata)	
	{
		snprintf(ddata->name, sizeof(ddata->name), string);
		ddata->gpio = devm_gpiod_get(&pdev->dev, string, GPIOD_OUT_HIGH);
		if (ddata->gpio > 0)
		{
			gpiod_direction_input(ddata->gpio);
			ddata->irq = gpiod_to_irq(ddata->gpio);
		}
		if (ddata->irq > 0)
			devm_request_irq(&pdev->dev, ddata->irq, my_irq_handler, IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, THIS_MODULE->name, ddata);
	}

	return 0;
}

static int my_remove(struct platform_device *pdev)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)dev_get_platdata(&pdev->dev);
	struct miscdevice *misc = (struct miscdevice *)dev_get_drvdata(&pdev->dev);
	if (misc)
	{
		misc_deregister(misc);
	}
	if (ddata->irq)
		devm_free_irq(&pdev->dev, ddata->irq, ddata);
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
