obj-m += bbq10kbd.o

export KROOT=/lib/modules/$(shell uname -r)/build

all:  modules

modules modules_install clean::
	@$(MAKE) -C $(KROOT) M=$(shell pwd) $@

clean::
	rm -rf   Module.symvers modules.order


push:
	adb push bbq10kbd.c /root/bbq10kbd-kernel-driver/
	adb push dts/bbq10kbd-radxa-zero.dts /root/bbq10kbd-kernel-driver/dts
	adb shell /root/bbq10kbd-kernel-driver/build-radxa.sh 


log: 
	adb wait-for-device && adb shell dmesg -wH
