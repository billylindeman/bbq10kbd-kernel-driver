# bbq10 Linux i2c keyboard driver

A Linux i2c keyboard driver for the bbq10 keyboard.  This can be used with a raspberry pi or radxa zero.

This kernel driver interfaces with the i2c protocol in this repo: https://github.com/solderparty/bbq10kbd_i2c_sw



###Compile Device Tree Overlay
```
dtc -@ -I dts -O dtb -o dts/bbq10kbd-radxa-zero.dtbo bbq10kbd-radxa-zero.dts
```



## Radxa Zero Setup

### Keyboard Driver

##### Build Driver
```
cd ~
git clone https://github.com/billylindeman/bbq10kbd-kernel-driver 
cd bbq10kbd-kernel-driver/
make modules modules_install
depmod -A
echo bbq10kbd >> /etc/modules
```

##### Setup Devicetree Overlay
```
dtc -@ -I dts -O dtb -o bbq10kbd-radxa-zero.dtbo dts/bbq10kbd-radxa-zero.dts 
cp bbq10kbd-radxa-zero.dtbo /boot/dtbs/5.10.69-12-amlogic-g98700611d064/amlogic/overlay/
```

Add these devicetree overlay's to `/boot/uEnv.txt`
```
overlays=meson-g12a-i2c-ee-m3-gpioa-14-gpioa-15 bbq10kbd-radxa-zero
```
