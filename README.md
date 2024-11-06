## Flash

 `python esptool.py -p /dev/tty.usbserial-0001 -b 460800 --before default_reset --chip esp32 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin 0x290000 littlefs.bin`

## USB

```cpp

// Default USB Settings
#define USB_VID 			0x303B
#define USB_PID 			0xA110
#define USB_MANUFACTURER 	"ALLUVIAL"
#define USB_PRODUCT 		"ESP32-S2-Joystick"
#define USB_SERIAL 			"0"

```