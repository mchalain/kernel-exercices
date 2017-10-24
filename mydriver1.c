/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */

MODULE_DESCRIPTION("mydriver1");
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
	pr_info("Hello World\n");
	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Goodbye wicked World\n");
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
