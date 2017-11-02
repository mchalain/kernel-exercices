/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/fs.h>           /* file_operations */
#include <linux/cdev.h>
#include <linux/device.h>

#include "myclass.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */
static short int my_minor = 0;

static int my_funcr(void *arg)
{
	pr_info("hello read\n");
	return 0;
}

static int my_funcw(void *arg)
{
	pr_info("hello write\n");
	return 0;
}

static struct myclass_s my_fops = {
	.funcw =		my_funcw,
	.funcr =		my_funcr,
};

static int __init my_init(void)
{
	my_minor = myclass_register(&my_fops, "mydriver", NULL);
	return 0;
}

static void __exit my_exit(void)
{
	myclass_unregister(my_minor);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
