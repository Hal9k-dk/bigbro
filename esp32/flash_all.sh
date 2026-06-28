#!/bin/sh
esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=no-reset --no-stub --after=hard-reset write-flash --flash-mode dio --flash-freq 40m --flash-size 4MB 0x1000 build/bootloader/bootloader.bin 0x10000 build/bigbro.bin 0x8000 build/partition_table/partition-table.bin 0xd000 build/ota_data_initial.bin 0x150000 build/font.bin
