# Run this Makefile as follows:
# (MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
#

KDIR= /lib/modules/$(shell uname -r)/build
PWD= $(shell pwd)
KBUILD_EXTRA_SYMBOLS=$(KDIR)/Module.symvers

obj-m += mydriver1.o
obj-m += mydriver2.o

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) KBUILD_EXTRA_SYMBOLS=$(KBUILD_EXTRA_SYMBOLS) modules

install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install
	depmod -a

clean:
	rm -f *~ Module.markers Modules.symvers
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

