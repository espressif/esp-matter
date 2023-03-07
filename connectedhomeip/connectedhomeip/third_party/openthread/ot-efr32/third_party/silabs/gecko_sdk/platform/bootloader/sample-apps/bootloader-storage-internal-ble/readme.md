# Bluetooth In Place OTA DFU

This sample configuration of the Gecko bootloader configures the bootloader to use the internal main flash to store firmware update images. The storage configuration is set up to store a single firmware update image at a time, in a single storage slot. The storage slot is configured to start at address 0x21800 (or 0x8021800 for device with 0x8000000 flash base), and have a size of 86 kB. This can be configured on the *"Bootloader Storage Slot Setup"*.

See *UG489: Gecko Bootloader User's Guide* to learn more about how to configure the bootloader, configure storage layout, enable security features, etc.
