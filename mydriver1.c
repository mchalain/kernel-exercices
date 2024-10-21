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

#include <linux/regmap.h>

#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */

static unsigned long offset = 0;
module_param(offset, ulong, 0644);

static LIST_HEAD(mydevice_list);
static int mydevice_nb = 0;

/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)file->private_data;
	dev_info(&ddata->pdev->dev, "read()\n");
	int length = 0;
	uint32_t value;
	regmap_read(ddata->regmap, offset, &value);
	length = snprintf(buf, count, "reg %#lX %#X\n", ddata->memregion.start + offset, value);
	if (ddata->read >= length)
		return 0;
	dev_info(&ddata->pdev->dev, "read(%#lX, %#X)\n", offset, value);
	count = length;
	*ppos += count;
	ddata->read += count;
	return count;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)file->private_data;
	dev_info(&ddata->pdev->dev, "write()\n");
	if (count >= 9)
	{
		uint32_t value = 0;
		for (int i = 0; i < 8; i++)
		{
			if (buf[i] > 0x29 && buf[i] < 0x40)
				value |= (buf[i] - 0x30) & 0x0F;
			if (buf[i] >= 'a' && buf[i] < 'g')
				value |= (buf[i] - 'a') & 0x0F;
			if (buf[i] >= 'A' && buf[i] < 'G')
				value |= (buf[i] - 'A') & 0x0F;
			value <<= 4;
		}
		dev_info(&ddata->pdev->dev, "write(%#lX, %#X)\n", offset, value);
		regmap_write(ddata->regmap, offset, value);
	}
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
		ddata->read = 0;
		file->private_data = ddata;
		printk(KERN_INFO "my char driver: open()\n");
		return 0;
	}
	return EBUSY;
}

static int my_release(struct inode *inode, struct file *file)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)file->private_data;
	dev_info(&ddata->pdev->dev, "release()\n");
	return 0;
}

static struct file_operations my_fops = {
	.owner =	THIS_MODULE,
	.read =		my_read,
	.write =	my_write,
	.open =		my_open,
	.release =	my_release,
};

static const struct regmap_config myregmap_config =
{
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
};

static int my_probe(struct platform_device *pdev)
{
	struct mydriver1_data_t *ddata;
	ddata = devm_kzalloc(&pdev->dev, sizeof(*ddata), GFP_KERNEL);
	if (ddata)
	{
		ddata->pdev = pdev;
		dev_info(&pdev->dev, "probe ddata %p\n",ddata);

		struct miscdevice *misc = devm_kmalloc(&pdev->dev, sizeof(*misc), GFP_KERNEL);
		snprintf(ddata->name, sizeof(ddata->name), "mydriver%d", mydevice_nb % 100);
		mydevice_nb++;
		misc->minor = MISC_DYNAMIC_MINOR;
		misc->name = ddata->name;
		misc->fops = &my_fops;

		misc_register(misc);
		dev_info(&pdev->dev, "misc minor %d\n", misc->minor);
		dev_set_drvdata(&pdev->dev, ddata);
		ddata->misc = misc;
		list_add(&ddata->list, &mydevice_list);

		void __iomem *registers = devm_platform_ioremap_resource(pdev, 0);
		if (registers)
			ddata->regmap = regmap_init_mmio(&pdev->dev, registers, &myregmap_config);
	}
	const char *string = NULL;
	struct device_node *node = ddata->pdev->dev.of_node;
	if (of_find_property(node, "chipname", NULL) == NULL)
	{
		node  = of_find_node_with_property(node, "chipname");
	}
	of_property_read_string(node, "chipname", &string);
	if (string)
	{
		dev_info(&pdev->dev, "access to %s\n", string);
	}

	return 0;
}

static int my_remove(struct platform_device *pdev)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)dev_get_drvdata(&pdev->dev);
	if (ddata->misc)
	{
		misc_deregister(ddata->misc);
	}
	if (ddata->regmap)
		regmap_exit(ddata->regmap);
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
