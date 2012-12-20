ifneq ($(KERNELRELEASE),)
	obj-m := rt.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	rm -rf .tmp_versions .rt.ko.cmd .rt.mod.o.cmd .rt.o.cmd rt.o rt.mod.c rt.mod.o modules.order Module.symvers

clean:
	rm rt.ko

endif
