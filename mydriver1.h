#ifndef _MYDRIVER1_H
#define _MYDRIVER1_H

#define MYIO_IOC_MAGIC	'k'
#define MYIO_STATUS		_IO(MYIO_IOC_MAGIC, 127)
#define MYIO_SET		_IOW(MYIO_IOC_MAGIC, 128, uint32_t)
#define MYIO_GET		_IOR(MYIO_IOC_MAGIC, 129, uint32_t)

#endif
