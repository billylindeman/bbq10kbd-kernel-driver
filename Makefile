obj-m += bbq10kbd.o

export KROOT=/lib/modules/$(shell uname -r)/build

all:  modules

modules modules_install clean::
	@$(MAKE) -C $(KROOT) M=$(shell pwd) $@

clean::
	rm -rf   Module.symvers modules.order

keymap:
	mkdir -p /usr/share/keymaps/blackberry/
	gzip < bbq10kbd.kmap > /usr/share/keymaps/blackberry/bbq10kbd.kmap.gz

keymap_config:
	echo "KMAP=/usr/share/keymaps/blackberry/bbq10kbd.kmap.gz" >> /etc/default/keyboard

push:
	adb push Makefile /root/bbq10kbd-kernel-driver/
	adb push bbq10kbd.c /root/bbq10kbd-kernel-driver/
	adb push bbq10kbd.h /root/bbq10kbd-kernel-driver/
	adb push bbq10kbd_keycodes.h /root/bbq10kbd-kernel-driver/
	adb push build-radxa.sh /root/bbq10kbd-kernel-driver/
	adb push dts/bbq10kbd-radxa-zero.dts /root/bbq10kbd-kernel-driver/dts
	adb shell /root/bbq10kbd-kernel-driver/build-radxa.sh 

push_keymap:
	adb push bbq10kbd.kmap /root/bbq10kbd-kernel-driver/
	adb shell 'gzip < /root/bbq10kbd-kernel-driver/bbq10kbd.kmap > /usr/share/keymaps/blackberry/bbq10kbd.kmap.gz'
	adb shell loadkeys bbq10kbd

log: 
	adb wait-for-device && adb shell dmesg -wH
