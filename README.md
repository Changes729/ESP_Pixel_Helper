## Flash

 `python esptool.py -p /dev/tty.usbserial-0001 -b 460800 --before default_reset --chip esp32 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin 0x290000 littlefs.bin`