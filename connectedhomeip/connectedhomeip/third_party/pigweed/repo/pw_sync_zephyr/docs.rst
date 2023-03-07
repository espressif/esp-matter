.. _module-pw_sync_zephyr:

================
pw_sync_zephyr
================

--------
Overview
--------
This sync backend implements the ``pw_sync`` facade. To enable, set
``CONFIG_PIGWEED_SYNC=y``. After that, specific submodules can be enabled via
the Kconfig menu.

* ``pw_sync.mutex`` can be enabled via ``CONFIG_PIGWEED_SYNC_MUTEX``.
* ``pw_sync.binary_semaphore`` can be enabled via
  ``CONFIG_PIGWEED_SYNC_BINARY_SEMAPHORE``.
