#!/bin/bash

echo "BUILDING KERNEL DRIVER"
cd /root/bbq10kbd-kernel-driver 
make modules modules_install

echo "BUILDING DEVICE TREE OVERLAY"
dtc -@ -I dts -O dtb -o bbq10kbd-radxa-zero.dtbo dts/bbq10kbd-radxa-zero.dts 
cp bbq10kbd-radxa-zero.dtbo /boot/dtbs/5.10.69-12-amlogic-g98700611d064/amlogic/overlay/
reboot
