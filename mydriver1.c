/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

static char my_str[256] = "Enter in a function of mydriver1";
module_param_string(message, my_str, sizeof(my_str), 0644);
MODULE_PARM_DESC(message,"Une chaîne de caractères");

/*
 * Arguments
 */

int mydriver1_print(void)
{
	printk(KERN_INFO"%s\n", my_str);
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
