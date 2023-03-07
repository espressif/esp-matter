====== Silabs Kit Platform ======

This package contains BSP functionality, basic drivers and configuration header
files for all Silicon Labs starter kits and development kits.

This package requires C99 support.

====== File structure ======

common/bsp
   C source and header files for kit specific functionality, such as
   enabling kit specific peripherals to be accessible from the mcu.
   (configures on board analog switches - that are there to prevent
   current leakage, gives access to LEDs, dip switches, joystick, i2c
   devices and so on).

common/drivers
   Various drivers for kit specific components.

<kit-name>/config
   Configuration data for BSP and Drivers in common/bsp and common/drivers.

====== License ======

License information for use of the source code is given at the top of
all C/H files.

Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com
