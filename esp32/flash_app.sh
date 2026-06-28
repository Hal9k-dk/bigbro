#!/bin/sh
esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=no-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 40m --flash-size 4MB 0x10000 build/bigbro.bin
