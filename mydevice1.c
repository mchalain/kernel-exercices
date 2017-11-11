#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

MODULE_DESCRIPTION("mydriver1");
MODULE_AUTHOR("Marc Chalain, Smile ECS");
MODULE_LICENSE("GPL");

static struct resource my_resource[] = {
	[0] = {
		.name = "my_resource",
		.start = 7,
		.flags = IORESOURCE_IRQ,
	},
};

static void my_release(struct device *dev)
{
}

static struct platform_device my_device =
{
	.name = "mydriver1",
	.dev = {
		.release = my_release,
	},
	.resource = my_resource,
	.num_resources = ARRAY_SIZE(my_resource),
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
