# SPI Flash Storage Bootloader (Single Image with Slot Size of 1024 KB)

This application requires external SPI Flash of at least 1 MB. This sample configuration of the Gecko bootloader configures the bootloader to use an external SPI flash to store firmware update images. The storage configuration is set up to store a single firmware update image at a time, in a single storage slot. The storage slot is configured to start at address 0x0 of the SPI flash, and have a size of 1024 kB. This can be configured on the *"Bootloader Storage Slot Setup"*.

For information about multiple storage slot support, see the "SPI Flash Storage Bootloader (multiple images)" sample configuration.

See *UG489: Gecko Bootloader User's Guide* to learn more about how to configure the bootloader, configure storage layout, enable security features, etc.
