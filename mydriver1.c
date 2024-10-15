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

#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */

static LIST_HEAD(mydevice_list);
static int mydevice_nb = 0;


static void print_device_tree_node(struct device_node *node, int depth);
/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct mydriver1_data_t *data = (struct mydriver1_data_t *)file->private_data;
	int length = 0;
	if (data->string)
	{
		length = strlen(data->string);
		printk(KERN_INFO "my char driver: read(%d) => %s\n", length, data->string);

		if (data->read >= length)
			return 0;
		if (copy_to_user(buf, data->string, length) == length)
		{
			buf[length] = '\n';
			length++;
			buf[length] = '\0';
		}
	}
	count = length;
	*ppos += count;
	data->read += count;
	return count;
}

static ssize_t my_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "my char driver: write()\n");
	print_device_tree_node(of_find_node_by_path("/"), 3);
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

static void print_device_tree_node(struct device_node *node, int depth)
{
	int i = 0;
	struct device_node *child;
	struct property    *properties;
	char                indent[255] = "";

	for(i = 0; i < depth * 3; i++)
	{
		indent[i] = ' ';
	}
	indent[i] = '\0';
	++depth;

	for_each_child_of_node(node, child)
	{
		printk(KERN_INFO "%s{ name = %s\n", indent, child->name);
		printk(KERN_INFO "%s  full name = %s\n", indent, child->full_name);
		for (properties = child->properties; properties != NULL; properties = properties->next)
		{
			printk(KERN_INFO "%s  %s (%d)\n", indent, properties->name, properties->length);
		}
		print_device_tree_node(child, depth);
		printk(KERN_INFO "%s}\n", indent);
	}
}

static int my_probe(struct platform_device *dev)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)dev_get_platdata(&dev->dev);
	if (ddata == NULL)
	{
		struct device_node *node;
		node = of_find_node_with_property(NULL, "string");
		if (node)
		{
			ddata = kzalloc(sizeof(*ddata), GFP_KERNEL);
			of_property_read_string(node, "string", &ddata->string);
		}
	}
	pr_info("probe ddata %p\n",ddata);
	if (ddata)
	{
		struct miscdevice *misc = &ddata->misc;
		snprintf(ddata->name, sizeof(ddata->name), "mydriver%d", mydevice_nb % 100);
		mydevice_nb++;
		misc->minor = 110 + mydevice_nb;
		misc->name = ddata->name;
		misc->fops = &my_fops;

		misc_register(misc);

		list_add(&ddata->list, &mydevice_list);
	}
	return 0;
}

static int my_remove(struct platform_device *dev)
{
	struct mydriver1_data_t *ddata = (struct mydriver1_data_t *)dev->dev.platform_data;
	if (ddata)
	{
		misc_deregister(&ddata->misc);
		kvfree(ddata);
	}
	return 0;
}

struct of_device_id my_devices_table[] =
{
	{
		.compatible = "mydriver1",
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
