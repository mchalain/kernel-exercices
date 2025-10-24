/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/vmalloc.h>	/* memory allocation */
#include <linux/slab.h>		/* memory allocation kmalloc */
#include <asm/io.h>		/* physical memory management */

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

/*
 * External functions
 */
void my_gpio_set(int pin, int high);
/*
 * Arguments
 */

/*
 * exported functions
 */

/*
 * Modules initialization
 */
static int __init my_init(void)
{
	my_gpio_set(17, 1);
	return 0;
}

static void __exit my_exit(void)
{
	my_gpio_set(17, 0);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
