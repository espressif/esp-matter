# SPI Flash Storage Bootloader (Multiple Images)

This sample configuration of the Gecko bootloader configures the bootloader to use an external SPI flash to store firmware update images. The storage configuration is set up to store up to two firmware update images at a time, in two storage slots. The storage slots are configured to start at address 0x2000 and 0x41000 of the SPI flash respectively, and have a size of 252 kB. This can be configured on the *"Bootloader Storage Slot Setup"*.

The "bootload info" meta-information, which tells the bootloader which storage slot to install the next firmware update from, is configured under *"Bootloader Storage Slot Setup"* to be placed at address 0x0. The first storage slot is configured to start at address 0x2000 (8 kB) because the bootload info takes up 2 flash pages of 4 kB.

See *UG489: Gecko Bootloader User's Guide* to learn more about how to configure the bootloader, configure storage layout, enable security features, etc.
