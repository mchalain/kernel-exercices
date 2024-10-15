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

#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */

static LIST_HEAD(mydevice_list);
static int mydevice_nb = 0;

static char *resourcename = "my_resource";
module_param(resourcename, charp, S_IRUGO|S_IWUSR);
static int resourcetype = IORESOURCE_IRQ;
module_param(resourcetype, int, S_IRUGO|S_IWUSR);
static char *resourceType = "IRQ";
module_param(resourceType, charp, S_IRUGO|S_IWUSR);
/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct mydriver1_data_t *data = (struct mydriver1_data_t *)file->private_data;
	struct resource *res = data->res;
	void *address = (void *)res->start;
	uint32_t size = resource_size(res);
	ssize_t ret = 0;
	printk(KERN_INFO "my char driver: read() => %p\n", address);

	if (data->read > 0)
		return ret;

	ret += snprintf(buf, count, "res: %s\n", res->name);
	switch (res->flags & IORESOURCE_TYPE_BITS)
	{
		case IORESOURCE_REG:
		case IORESOURCE_MEM:
			ret += snprintf(buf + ret, count - ret, "Mem 0x%p/%u\n", address, size);
		break;
		case IORESOURCE_IO:
			ret += snprintf(buf + ret, count - ret, "IOP 0x%p/%u\n", address, size);
		break;
		case IORESOURCE_IRQ:
			ret += snprintf(buf + ret, count - ret, "Irq %u\n", (unsigned int)res->start);
		break;
		case IORESOURCE_DMA:
			ret += snprintf(buf + ret, count - ret, "Dma %u\n", (unsigned int)res->start);
		break;
		case IORESOURCE_BUS:
			ret += snprintf(buf + ret, count - ret, "Bus %u\n", (unsigned int)res->start);
		break;
	}
	*ppos += ret;
	data->read += ret;
	return ret;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "my char driver: write()\n");

	*ppos += count;
	return count;
}

static int my_open(struct inode *inode, struct file *file)
{
	struct mydriver1_data_t *data = NULL, *pos;
	list_for_each_entry(pos, &mydevice_list, list)
	{
		if (pos->misc.minor == iminor(inode))
		{
			data = pos;
			break;
		}
	}
	if (data)
	{
		data->read = 0;
		file->private_data = data;
		printk(KERN_INFO "my char driver: open()\n");
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
	struct resource *res = NULL;
	struct mydriver1_data_t *data = NULL;
	pr_info("dev %s (%s)\n", pdev->name, pdev->resource[0].name);
	res = platform_get_resource_byname(pdev, resourcetype, resourcename);

	if (res)
	{
		data = kzalloc(sizeof(*data), GFP_KERNEL);
		snprintf(data->name, sizeof(data->name), "mydriver%d", mydevice_nb % 100);
		mydevice_nb++;
		data->misc.minor = MISC_DYNAMIC_MINOR;
		data->misc.name = data->name;
		data->misc.fops = &my_fops;

		misc_register(&data->misc);

		data->res = res;
		list_add(&data->list, &mydevice_list);
	}

	platform_set_drvdata(pdev, data);
	return 0;
}

static int my_remove(struct platform_device *pdev)
{
	struct mydriver1_data_t *data = platform_get_drvdata(pdev);

	if (data)
	{
		pr_info("remove drvdata %p\n",data);
		if (data->misc.minor > 0)
		{
			misc_deregister(&data->misc);
		}
		kvfree(data);
	}

	return 0;
}

static struct platform_driver my_driver =
{
	.driver = {
		.name = "mydriver1",
	},
	.probe = my_probe,
	.remove = my_remove,
};


static int __init my_init(void)
{
	if (!strcmp(resourceType, "MEM"))
		resourcetype = IORESOURCE_MEM;
	else if (!strcmp(resourceType, "IO"))
		resourcetype = IORESOURCE_IO;
	else if (!strcmp(resourceType, "REG"))
		resourcetype = IORESOURCE_REG;
	else if (!strcmp(resourceType, "DMA"))
		resourcetype = IORESOURCE_DMA;
	else if (!strcmp(resourceType, "BUS"))
		resourcetype = IORESOURCE_BUS;
	else
		resourcetype = IORESOURCE_IRQ;
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
