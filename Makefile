obj-m += bbq10.o
BBQ10_FLAGS += -g -DDEBUG
ccflags-y += ${BBQ10_FLAGS}
CC += ${BBQ10_FLAGS}

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
debug:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	EXTRA_FLAGS="${BBQ10_FLAGS}"
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install:
	sudo cp bbq10.ko /lib/modules/$(shell uname -r)/kernel/bbq10/bbq10.ko
