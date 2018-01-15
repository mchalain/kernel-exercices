#ifndef __MYDRVIER1_H__
#define __MYDRVIER1_H__

struct mydriver1_data_t
{
	char name[11];
	struct miscdevice misc;
	struct device_node *node;
	int read;
	struct list_head list;
};

#endif
