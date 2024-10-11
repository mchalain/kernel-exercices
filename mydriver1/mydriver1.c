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

int mydriver1_print(void)
{
	printk(KERN_INFO"Enter in a function of mydriver1\n");
	return 0;
}
EXPORT_SYMBOL(mydriver1_print);

/*
 * Modules initialization
 */
static int __init my_init(void)
{
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
