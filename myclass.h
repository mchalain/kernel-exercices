#ifndef __MYCLASS_H_
#define __MYCLASS_H_

struct myclass_s
{
	int (*func)(void);
};

int myclass_register(struct myclass_s *data);
void myclass_unregister(int minor);

#endif
