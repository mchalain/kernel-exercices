/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */

#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver2");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * Arguments
 */

/*
 * Modules initialization
 */
static int __init my_init(void)
{
	mydriver1_print();
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
