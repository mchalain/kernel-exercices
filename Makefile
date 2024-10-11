KDIR= /lib/modules/$(shell uname -r)/build
PWD= $(shell pwd)
KBUILD_EXTRA_SYMBOLS=$(PWD)/mydriver1/Module.symvers

all: mydriver1 mydriver2

.PHONY:mydriver1 mydriver2
mydriver1:
	$(MAKE) -C $(PWD)/mydriver1

mydriver2:
	$(MAKE) -C $(PWD)/mydriver2

install:
	$(MAKE) -C $(PWD)/mydriver1 install
	$(MAKE) -C $(PWD)/mydriver2 install

clean:
	$(MAKE) -C $(PWD)/mydriver1 clean
	$(MAKE) -C $(PWD)/mydriver2 clean

