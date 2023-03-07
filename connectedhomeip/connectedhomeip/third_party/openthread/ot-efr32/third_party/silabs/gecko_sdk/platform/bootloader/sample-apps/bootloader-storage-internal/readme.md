# Internal Storage Bootloader (Multiple Images on 1 MB Device)

This is an application bootloader for all EFR32 and EFM32 devices with 1 MB or larger flash, using the internal flash memory to store upgrade images received by the application over the air in an application-specific way. This sample configuration supports storing multiple firmware update images simultaneously by configuring multiple storage slots. The storage layout is designed for 1024 kB EFR32xG12 devices, and the layout of the storage should be modified before being run on other devices.

Bootloader_core: This sample configuration of the Gecko bootloader configures the bootloader to use the internal main flash to store firmware update images. The storage configuration is set up to store up to two firmware update images at a time, in two storage slots.

This sample configuration is designed for use on 1024 kB EFR32xG12 devices, and needs to be re-configured to be used on other devices.

This sample configuration is designed for use with the following memory layout:
**Address**   
0 - 320 kB Application   
320 kB - 356 kB reserved for SimEE (1 or 2) NVM   
356 kB - 360 kB Bootload Info   
360 kB - 690 kB Storage slot 0   
690 kB - 1020 kB Storage slot 1   
1020 kB - 1024 kB reserved for PStore NVM  

To use the sample bootloader configuration, build your applications to account for part of the main flash, which is used for firmware update image storage.

The "bootload info" meta-information, which tells the bootloader which storage slot to install the next firmware update from, is configured under "Components->Common Storage" to be placed at address 0x59000 (356 kB). The first storage slot is configured to start at address 0x5A000 (360 kB), because the bootload info takes up 2 flash pages of 2 kB.

See *UG489: Gecko Bootloader User's Guide* to learn more about how to configure the bootloader, configure storage layout, enable security features, etc.
