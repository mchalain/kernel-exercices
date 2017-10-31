#ifndef _MYDRIVER1_H
#define _MYDRIVER1_H

#define RPI_GPIO_IOC_MAGIC	'k'
#define RPI_GPIO_SET		_IOW(RPI_GPIO_IOC_MAGIC, 128, short)
#define RPI_GPIO_CLEAR		_IOW(RPI_GPIO_IOC_MAGIC, 129, short)
#define RPI_GPIO_GET		_IOR(RPI_GPIO_IOC_MAGIC, 130, uint32_t)

#endif
