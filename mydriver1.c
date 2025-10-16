/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/vmalloc.h>	/* memory allocation */

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

static int my_size = 9;
module_param_named(message_size, my_size, int, 0644);
MODULE_PARM_DESC(message_size,"La taille du message");

static char *my_str = NULL;
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
	my_str = vmalloc(my_size);
	memset(my_str, 'A', my_size);
	printk(KERN_INFO"Memory allocated at %p\n", my_str);
	return 0;
}

static void __exit my_exit(void)
{
	vfree(my_str);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
