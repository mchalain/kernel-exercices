#ifndef __MYDRVIER1_H__
#define __MYDRVIER1_H__

struct mydriver1_data_t
{
	char name[11];
	struct miscdevice *misc;
	struct platform_device *pdev;
	int read;
	struct list_head list;
	struct { unsigned long int start; unsigned long int end; void *mem;} memregion;
	struct regmap *regmap;
};

#endif
