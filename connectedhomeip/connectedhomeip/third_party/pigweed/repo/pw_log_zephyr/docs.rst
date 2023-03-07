.. _module-pw_log_zephyr:

================
pw_log_zephyr
================

--------
Overview
--------
This interrupt backend implements the ``pw_log`` facade. To enable, set
``CONFIG_PIGWEED_LOG=y``. After that, logging can be controlled via the standard
`Kconfig options <https://docs.zephyrproject.org/latest/reference/logging/index.html#global-kconfig-options>`_.
All logs made through `PW_LOG_*` are logged to the Zephyr logging module
``pigweed``.

Setting the log level
---------------------
In order to remain compatible with existing Pigweed code, the logging backend
respects ``PW_LOG_LEVEL``. If set, the backend will translate the Pigweed log
levels to their closest Zephyr counterparts:

+---------------------------+-------------------+
| Pigweed                   | Zephyr            |
+===========================+===================+
| ``PW_LOG_LEVEL_DEBUG``    | ``LOG_LEVEL_DBG`` |
+---------------------------+-------------------+
| ``PW_LOG_LEVEL_INFO``     | ``LOG_LEVEL_INF`` |
+---------------------------+-------------------+
| ``PW_LOG_LEVEL_WARN``     | ``LOG_LEVEL_WRN`` |
+---------------------------+-------------------+
| ``PW_LOG_LEVEL_ERROR``    | ``LOG_LEVEL_ERR`` |
|                           |                   |
| ``PW_LOG_LEVEL_CRITICAL`` |                   |
|                           |                   |
| ``PW_LOG_LEVEL_FATAL``    |                   |
+---------------------------+-------------------+

Alternatively, it is also possible to set the Zephyr logging level directly via
``CONFIG_PIGWEED_LOG_LEVEL``.
