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
/*
 * File operations
 */

/**
 * I2C functions
 */
static int __init my_init(void)
{
	struct i2c_adapter *adapter;

	adapter = i2c_get_adapter(1);
	i2c_new_device(adapter, &my_boardinfo);

	return 0;
}

static void __exit my_exit(void)
{
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
