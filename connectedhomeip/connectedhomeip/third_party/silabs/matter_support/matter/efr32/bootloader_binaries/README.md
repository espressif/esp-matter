# Bootloader binaries for Matter

This directory contains Silicon Labs Gecko Bootloader binaries that can be used with Matter OTA Software Update functionality.

- The external storage (SPI flash) bootloaders are configured with the slot size of 1024K 
- The internal storage bootloaders (supported on MG24 cards only) are configured as follows:
  - Slot Address: 135184384 (0x080ec000)
  - Slot Size: 548864
- All bootloaders supports LZMA compression
- For MG12 boards use the <name>-combined.s37 (e.g. bootloader-storage-spiflash-single-1024k-BRD4161A-gsdk4.1-combined.s37) bootloader version the first time a device is programmed, whether during development or manufacturing. For subsequent programming, if the "combined" bootloader had been previously flashed to the device use the regular version (<name>.s37, e.g. bootloader-storage-spiflash-single-1024k-BRD4161A-gsdk4.1.s37). 
- The bootloaders are compatible with Matter applications built with GSDK 4.1. Bootloaders for Matter applications built with GSDK 3.1 can be found here: https://github.com/SiliconLabs/sdk_support/tree/cf743ca3cca307ddb209c45f62aed7dc5d8ce06d/platform/bootloader/sample-apps/bootloader-storage-spiflash-single .


