.. _module-pw_sys_io_zephyr:

================
pw_sys_io_zephyr
================

--------
Overview
--------
This sys I/O backend implements the ``pw_sys_io`` facade. To enable, set
``CONFIG_PIGWEED_SYS_IO=y``. Once enabled, I/O operations will be routed to
Zephyr's console. Additionally, it is possible to enable the USB subsystem
by setting ``CONFIG_PIGWEED_SYS_IO_USB=y``.

The I/O backend initializes during Zephyr's ``APPLICATION`` level and uses
``CONFIG_PIGWEED_SYS_IO_INIT_PRIORITY`` to set the priority level. This config
value defaults to 1, but is configurable via Kconfig.
