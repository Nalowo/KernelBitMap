KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
BUILD := $(abspath build)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) MO=$(BUILD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) MO=$(BUILD) clean

load: all
	sudo insmod $(BUILD)/kernel_alloc.ko

unload:
	sudo rmmod kernel_alloc_module

format:
	clang-format -i $(PWD)/src/*

.PHONY: all clean load unload format
