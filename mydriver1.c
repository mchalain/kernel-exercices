/*
 * Includes
 */
#include <linux/kernel.h>	/* printk() */
#include <linux/module.h>	/* modules */
#include <linux/vmalloc.h>	/* memory allocation */
#include <linux/slab.h>		/* memory allocation kmalloc */

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

static int my_size = 9;
module_param_named(message_size, my_size, int, 0644);
MODULE_PARM_DESC(message_size,"La taille du message");

static char *my_str[10] = {0};
/*
 * Arguments
 */

int mydriver1_print(void)
{
	printk(KERN_INFO"%s\n", my_str[0]);
	printk(KERN_INFO"%s\n", my_str[1]);
	printk(KERN_INFO"%s\n", my_str[2]);
	printk(KERN_INFO"%s\n", my_str[3]);
	printk(KERN_INFO"%s\n", my_str[4]);
	return 0;
}
EXPORT_SYMBOL(mydriver1_print);

/*
 * Modules initialization
 */
static int __init my_init(void)
{
	my_str[0] = vmalloc(my_size);
	memset(my_str[0], 'A', my_size);
	printk(KERN_INFO"vmalloc allocated at        %p\n", my_str[0]);
	my_str[5] = vmalloc(my_size);
	memset(my_str[5], 'F', my_size);
	printk(KERN_INFO"vmalloc 2 allocated at      %p\n", my_str[5]);
	my_str[1] = kmalloc(my_size, GFP_KERNEL);
	memset(my_str[1], 'B', my_size);
	printk(KERN_INFO"kmalloc KERNEL allocated at %p\n", my_str[1]);
	my_str[2] = kmalloc(my_size, GFP_USER);
	memset(my_str[2], 'C', my_size);
	printk(KERN_INFO"kmalloc USER allocated at   %p\n", my_str[2]);
	my_str[6] = kmalloc(my_size, GFP_USER);
	memset(my_str[6], 'G', my_size);
	printk(KERN_INFO"kmalloc USER 2 allocated at %p\n", my_str[6]);
	my_str[3] = kmalloc(my_size, GFP_DMA);
	memset(my_str[3], 'D', my_size);
	printk(KERN_INFO"kmalloc DMA allocated at    %p\n", my_str[3]);
	my_str[4] = kmalloc(my_size, GFP_ATOMIC);
	memset(my_str[4], 'E', my_size);
	printk(KERN_INFO"kmalloc ATOMIC allocated at %p\n", my_str[4]);
	return 0;
}

static void __exit my_exit(void)
{
	vfree(my_str[0]);
	vfree(my_str[5]);
	kfree(my_str[1]);
	kfree(my_str[2]);
	kfree(my_str[6]);
	kfree(my_str[3]);
	kfree(my_str[4]);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
