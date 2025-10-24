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

#define BCM2708_PERI_BASE       0x20000000
#define BCM2710_PERI_BASE       0x3F000000
#define BCM2711_PERI_BASE       0x3F000000
#define GPIO_BASE_OFFSET	0x200000

#define GPIO_BASE_ADDR			(BCM2710_PERI_BASE + GPIO_BASE_OFFSET)
#define GPIO_SIZE			0x40
#define GPIO_FUNCSELECT_REG(pin)	(0x00 + ((pin) / 10) * 4)
#define GPIO_FUNCSELECT_SHIFT(pin)	((pin % 10) * 3)
#define GPIO_FUNCSELECT_MASK		0x7
#define GPIO_LINE_SET(pin)		((0x1c / 4) + ((pin) / 32))
#define GPIO_LINE_CLEAR(pin)		((0x28 / 4) + ((pin) / 32))
#define GPIO_LINE_LEVEL(pin)		((0x34 / 4) + ((pin) / 32))
#define GPIO_LINE_SHIFT(pin)		(pin % 32)

/*
 * External functions
 */
void my_gpio_set(int pin, int high);
/*
 * Arguments
 */

u32 __iomem	*base = NULL;
/*
 * exported functions
 */
void my_gpio_set(int pin, int high)
{
	u32 reg = 0;
	u32 value = 0;
	int state = 0;

	// get the gpio direction to output
	reg = GPIO_FUNCSELECT_REG(pin);
	value = readl(base + reg);
	state = value >> GPIO_FUNCSELECT_SHIFT(pin) & 0x07;
	printk(KERN_INFO"gpio(%d) direction %s", pin, value?"out":"in"); 
	if (state != 1)
	{
		// set the gpio direction to input
		value &= ~(GPIO_FUNCSELECT_MASK << GPIO_FUNCSELECT_SHIFT(pin));
		value |= (0 << GPIO_FUNCSELECT_SHIFT(pin));
		writel(value, base + reg);
		// set the gpio direction to output
		value &= ~(GPIO_FUNCSELECT_MASK << GPIO_FUNCSELECT_SHIFT(pin));
		value |= (1 << GPIO_FUNCSELECT_SHIFT(pin));
		writel(value, base + reg);
		value = readl(base + reg);
		state = value >> GPIO_FUNCSELECT_SHIFT(pin) & 0x07;
		printk(KERN_INFO"gpio(%d) direction %s", pin, value?"out":"in"); 
	}
	if (high)
	{
		reg = GPIO_LINE_SET(pin);
	}
	else
	{
/*
 * External functions
 */
void my_gpio_set(int pin, int high);
		reg = GPIO_LINE_CLEAR(pin);
	}
	value = 1UL << GPIO_LINE_SHIFT(pin);
	writel(value, base + reg);
}
EXPORT_SYMBOL(my_gpio_set);

/*
 * Modules initialization
 */
static int __init my_init(void)
{
	base = ioremap(GPIO_BASE_ADDR, GPIO_SIZE);
	printk(KERN_INFO"io remap 0x%x | 0x%p\n", GPIO_BASE_ADDR, base);
	return 0;
}

static void __exit my_exit(void)
{
	iounmap(base);
}

/*
 * Module entry points
 */
module_init(my_init);
module_exit(my_exit);
