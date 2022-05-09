Zigbee Bridge
=============

1. Additional Environment Setup
-------------------------------

todo.

2. Post Commissioning Setup
---------------------------

todo.

3. Device Performance
---------------------

3.1 CPU and Memory usage
~~~~~~~~~~~~~~~~~~~~~~~~

The following is the CPU and Memory Usage.

-  ``Bootup`` == Device just finished booting up. Device is not
   commissionined or connected to wifi yet.
-  ``After Commissioning`` == Device is conneted to wifi and is also
   commissioned and is rebooted.
-  device used: esp32c3_devkit_m
-  tested on: `bd951b8 <https://github.com/espressif/esp-matter/commit/bd951b84993d9d0b5742872be4f51bb6c9ccf15e>`__ (2022-05-05)

======================== =========== ===================
\                        Bootup      After Commissioning
======================== =========== ===================
**Free Internal Memory** 109KB       105KB
**CPU Usage**            \-          \-
======================== =========== ===================

**Flash Usage**: Firmware binary size: 1.26MB

This should give you a good idea about the amount of CPU and free memory
that is available for you to run your application's code.

todo. Update the values.
