#ifndef __MYCLASS_H_
#define __MYCLASS_H_

int myclass_register(struct file_operations *fops, char *name, void *arg);
void myclass_unregister(int myminor);

#endif
