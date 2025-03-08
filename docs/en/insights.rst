Enabling ESP-Insights in ESP-Matter
===================================

- To learn more about esp-insights and get started, please refer `project README.md`_.
- Before building the app, enable the option `ESP_INSIGHTS_ENABLED` through menuconfig.
- Follow the steps present `set up esp-insights account`_ , and create an auth key.
- Create a file named `insights_auth_key.txt` in the project directory of the example.
- Download the auth key and copy Auth Key to the example.

   ::

      cp /path/to/auth/key.txt path/to/esp-matter/examples/generic_switch/insights_auth_key.txt

- Refer the esp-matter :project_file:`Generic Switch example <examples/generic_switch/main/app_main.cpp>` to enable the traces and metrics reported by the esp32 tracing backend in the chip SDK on the insights dashboard and about how to use the auth key for enabling insights.
- Enable the option `ENABLE_ESP_INSIGHTS_SYSTEM_STATS` to get a report of the system metrics in the chip SDK on the insights dashboard.

.. _project README.md: https://github.com/espressif/esp-insights/blob/main/README.md
.. _set up esp-insights account: https://github.com/espressif/esp-insights/blob/main/examples/README.md#set-up-esp-insights-account