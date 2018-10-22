/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <linux/miscdevice.h>
#include <linux/i2c.h>

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */
struct my_data_t
{
	struct miscdevice misc;
	struct i2c_client *client;
	int read;
};
/*
 * File operations
 */
static ssize_t my_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "my char driver: read()\n");

	if (*ppos >= 8)
		return 0;
	copy_to_user(buf, "Bonjour\n", 8);
	count = 8;
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
	vfree(file->private_data);

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

/**
 * I2C functions
 */
/**
 * method called by the probe of the I2C bus
 */
static int
my_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct my_data_t *data;
	pr_info("device %X\n", client->addr);

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	data->client = client;
	i2c_set_clientdata(client, data);

	data->misc.minor = MISC_DYNAMIC_MINOR;
	data->misc.name = "mydriver";
	data->misc.fops = &my_fops;

	misc_register(&data->misc);

	return 0;
}

static int
my_remove(struct i2c_client *client)
{
	struct my_data_t *data = i2c_get_clientdata(client);
	misc_deregister(&data->misc);
	kfree(data);
	return 0;
}

static const struct i2c_device_id my_ids[] = {
	{ "myi2c", 0, },
};

struct i2c_board_info my_boardinfo =
{
	I2C_BOARD_INFO("myi2c", 0x42),
};

static struct i2c_driver my_i2c = {
	.driver = {
		.name	= "mydriver",
	},
	.probe		= my_probe,
	.remove		= my_remove,
	.id_table	= my_ids,
};

static int __init my_init(void)
{
	struct i2c_adapter *adapter;

	i2c_register_driver(THIS_MODULE, &my_i2c);

	adapter = i2c_get_adapter(1);
	i2c_new_device(adapter, &my_boardinfo);

	return 0;
}

static void __exit my_exit(void)
{
	i2c_del_driver(&my_i2c);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
