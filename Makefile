# Run this Makefile as follows:
# (MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
#

KDIR= /lib/modules/$(shell uname -r)/build
PWD= $(shell pwd)
KBUILD_EXTRA_SYMBOLS=$(KDIR)/Module.symvers

obj-m += mydriver1.o

dtbo-y += mydevice1.dtbo
dtbo-y += mydevice2.dtbo

all: $(dtbo-y)
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) KBUILD_EXTRA_SYMBOLS=$(KBUILD_EXTRA_SYMBOLS) modules

install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install
	depmod -a

clean:
	rm -f *~ Module.markers Modules.symvers
	rm -f *.dtbo
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

%.dtbo: %.dts
	dtc -O dtb -o $@ -@ $<
