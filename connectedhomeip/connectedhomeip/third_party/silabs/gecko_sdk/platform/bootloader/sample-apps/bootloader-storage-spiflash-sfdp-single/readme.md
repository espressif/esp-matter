# SPI Flash Storage Bootloader Using SFDP (Single Image)

This sample configuration of the Gecko bootloader configures the bootloader to use an external SPI flash to store firmware update images. The SPI flash type is detected automatically by querying the SFDP parameter table present inside the flash memory. The storage configuration is set up to store a single firmware update image at a time, in a single storage slot. The storage slot is configured to start at address 0x0 of the SPI flash, and have a size of 512 kB. This can be configured on the *"Bootloader Storage Slot Setup"*.

For information about multiple storage slot support, see the "SPI Flash Storage Bootloader (multiple images)" sample configuration.

See *UG489: Gecko Bootloader User's Guide* to learn more about how to configure the bootloader, configure storage layout, enable security features, etc.
