# bbq10 Linux i2c keyboard driver

A Linux i2c keyboard driver for the bbq10 keyboard.  This can be used with a raspberry pi or radxa zero.

This kernel driver interfaces with the i2c protocol in this repo: https://github.com/solderparty/bbq10kbd_i2c_sw


###Compile Device Tree Overlay
```
dtc -@ -I dts -O dtb -o dts/bbq10kbd-radxa-zero.dtbo bbq10kbd-radxa-zero.dts
```



