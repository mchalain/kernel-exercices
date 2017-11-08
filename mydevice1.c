#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

#include "mydriver1.h"

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000)

static struct mydriver_data_t mydriver_data =
{
	.base = GPIO_BASE,
};

static struct platform_device my_device =
{
	.name = "mydriver1",
	.dev = {
		.platform_data = &mydriver_data,
	},
};

static int __init my_init(void)
{
	platform_device_register(&my_device);
	return 0;
}

static void __exit my_exit(void)
{
	platform_device_unregister(&my_device);
}

module_init(my_init);
module_exit(my_exit);
