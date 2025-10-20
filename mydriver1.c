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
 * Arguments
 */
static int my_size = 9;
static char *my_str = NULL;
static char *data = NULL;

/*
 * Modules initialization
 */
static int __init my_init(void)
{
	my_str = kmalloc(my_size, GFP_DMA);
	memset(my_str, 'B', my_size);
	printk(KERN_INFO"kmalloc DMA allocated at 0x%p | 0x%llx\n", my_str, virt_to_phys(my_str));

	void * phys_addr = virt_to_phys(my_str);
	kfree(my_str);

	data = ioremap(phys_addr, my_size);
	printk(KERN_INFO"io remap 0x%p | 0x%p\n", phys_addr, data);
	printk(KERN_INFO"data %s\n", data);
	return 0;
}

static void __exit my_exit(void)
{
	iounmap(data);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
