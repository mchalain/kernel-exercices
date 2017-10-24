/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>

#include "myclass.h"

MODULE_DESCRIPTION("myclass");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

struct myclass_storage_s
{
	struct myclass_s *fops;
	int data;
} storage;

/*
 * Arguments
 */
int myclass_register(struct myclass_s *data)
{
	storage.fops = data;
	storage.data++;

	if (storage.fops->func != NULL)
		storage.fops->func();
	return storage.data;
}
EXPORT_SYMBOL(myclass_register);

void myclass_unregister(int data)
{
	storage.fops = NULL;
}
EXPORT_SYMBOL(myclass_unregister);

static int __init my_init(void)
{
	storage.data = 0;
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
