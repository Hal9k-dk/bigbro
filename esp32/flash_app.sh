#!/bin/sh
esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=no_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 4MB 0x10000 build/bigbro2.bin
