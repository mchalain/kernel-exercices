KDIR= /lib/modules/$(shell uname -r)/build
PWD= $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install
	depmod -a

clean:
	rm -f *~ Module.markers Modules.symvers
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

