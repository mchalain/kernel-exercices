#ifndef __MYCLASS_H_
#define __MYCLASS_H_

struct myclass_s
{
	int (*funcw)(void *arg);
	int (*funcr)(void *arg);
	void *arg;
};

int myclass_register(struct myclass_s *fops, char *name, void *arg);
void myclass_unregister(int myminor);

#endif
