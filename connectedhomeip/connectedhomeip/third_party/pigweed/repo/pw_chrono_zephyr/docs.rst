.. _module-pw_chrono_zephyr:

================
pw_chrono_zephyr
================

--------
Overview
--------
This chrono backend implements the ``pw_chrono`` facades. To enable, set
``CONFIG_PIGWEED_CHRONO=y``.

pw_chrono.system_clock
----------------------
To enable the ``system_clock`` facade, it is also required to add
``CONFIG_PIGWEED_CHRONO_SYSTEM_CLOCK=y``.