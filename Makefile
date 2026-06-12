KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)
BUILD := $(abspath build)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) MO=$(BUILD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) MO=$(BUILD) clean

load: all
	sudo insmod $(BUILD)/kernel_alloc_module.ko

unload:
	sudo rmmod kernel_alloc_module

format:
	clang-format -i $(PWD)/src/*

check:
	sudo python3 check.py --ko $(BUILD)/kernel_alloc_module.ko

.PHONY: all clean load unload format
