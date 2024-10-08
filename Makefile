# Run this Makefile as follows:
# (MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
#

KDIR= /lib/modules/$(shell uname -r)/build
PWD= $(shell pwd)
#KBUILD_EXTRA_SYMBOLS=$(PWD)/Module.symvers

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install
	depmod -a

clean:
	rm -f *~ Module.markers Modules.symvers
	$(MAKE) -C $(KDIR) M=$(PWD) clean

