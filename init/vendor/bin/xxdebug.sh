#!/vendor/bin/sh
echo 255 > /sys/class/leds/breath/brightness
# ls -la /dev/block/bootdevice/by-name > /dev/block/sde54
# dmesg >> /dev/block/sde54 2>&1
/system/bin/logcat -b all >> /dev/block/sde54 2>&1
