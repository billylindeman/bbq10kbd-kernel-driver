obj-m += bbq10kbd.o
BBQ10KBD_FLAGS += -g -DDEBUG
ccflags-y += ${BBQ10KBD_FLAGS}
CC += ${BBQ10_FLAGS}

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
debug:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	EXTRA_FLAGS="${BBQ10KBD_FLAGS}"
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install:
	sudo cp bbq10.ko /lib/modules/$(shell uname -r)/kernel/bbq10/bbq10.ko
