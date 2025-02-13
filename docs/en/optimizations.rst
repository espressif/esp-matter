6. Configuration options to optimize RAM and Flash
==================================================

6.1 Overview
------------

There are several configuration options available to optimize Flash and RAM storage.
The following list highlights key options that significantly increase the free DRAM, heap, and reduce the flash
footprint.

For more optimizations, we've also listed the reference links to esp-idf's optimization guide.

6.2 Configurations
------------------

6.2.1 Test Environment setup
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All numbers mentioned below are collected in the following environment:

- esp-idf `v5.1.2`_
- esp-matter `cb3bc9d`_
- Example: `light`_
- SoC: ESP32-C3

.. note::

    - These numbers may vary slightly in a different environment.
    - All numbers are in bytes
    - As we are using BLE only for commissioning, BLE memory is freed post commissioning,
      hence there is an increase in the free heap post commissioning. (``CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING=y``)
    - After building an example, some DRAM will be utilized, and the remaining DRAM will be
      allocated as heap. Therefore, a direct increase in the free DRAM will reflect as an increase in free heap.


6.2.2 Default Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

We have used the default light example here, and below listed are the static and dynamic sizes.

.. csv-table:: Static memory stats
  :header: "", "Size", "Decreased by"

  Used D/IRAM,169042,--
  Used Flash,1370786,--

.. csv-table:: Dynamic memory stats
  :header: "", "Free Heap", "Increased by"

  On Bootup, 64824, --
  Post Commissioning, 126808, --


6.2.3 Disable the chip-shell
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Console shell is helpful when developing/debugging the application, but may not be necessary in
production. Disabling the shell can save space. Disable the below configuration option.

::

    CONFIG_ENABLE_CHIP_SHELL=n

.. csv-table:: Static memory stats
  :header: "", "Size", "Decreased by"

  Used D/IRAM,165480,3562
  Used Flash,1311926,58860

.. csv-table:: Dynamic memory stats
  :header: "", "Free Heap", "Increased by"

  On Bootup, 76344, 11520
  Post Commissioning, 137696, 10888


6.2.4 Adjust the dynamic endpoint count
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The default dynamic endpoint count is 16, which may be excessive for a normal application creating only 2 endpoints.
eg: light, only has two endpoints, one for root endpoint and one for actual light.
Adjusting this to a lower value, corresponding to the actual number of endpoints the application will create, can save DRAM.

Here, we have set the dynamic endpoint count to 4. Increase in the DRAM per endpoint is ~275 bytes.

::

    CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT=4

.. csv-table:: Static memory stats
  :header: "", "Size", "Decreased by"

  Used D/IRAM,162136,3344
  Used Flash,1311914,12

.. csv-table:: Dynamic memory stats
  :header: "", "Free Heap", "Increased by"

  On Bootup, 79688, 3344
  Post Commissioning, 141204, 3508


6.2.5 Use the newlib nano formatting
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This optimization saves approximately 25-50K of flash, depending on the target. In our case, it results in a flash
reduction of 61.5 KB.

Additionally, it lowers the high watermark of task stack for functions that call printf() or other string formatting
functions. Fore more details please take a look at esp-idf's `newlib nano formatting guide`_.

::

    CONFIG_NEWLIB_NANO_FORMAT=y

.. csv-table:: Static memory stats
  :header: "", "Size", "Decreased by"

  Used D/IRAM,162136,0
  Used Flash,1281354,30560

.. csv-table:: Dynamic memory stats
  :header: "", "Free Heap", "Increased by"

  On Bootup, 82748, 3060
  Post Commissioning, 143956, 2752


6.2.6 Few BLE Optimizations
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since most devices will primarily operate as BLE peripherals and typically won't need more than one connection
(especially if it's just a Matter app), we can optimize by reducing the maximum allowed connections, thereby
saving DRAM. Additionally, given the peripheral nature of these devices, we can disable the central and
observer roles, for further optimization.

Below are the configuration options that can be set to achieve these optimizations.

::

    CONFIG_NIMBLE_MAX_CONNECTIONS=1
    CONFIG_BTDM_CTRL_BLE_MAX_CONN=1
    CONFIG_BT_NIMBLE_ROLE_CENTRAL=n
    CONFIG_BT_NIMBLE_ROLE_OBSERVER=n

.. csv-table:: Static memory stats
  :header: "", "Size", "Decreased by"

  Used D/IRAM,161682,454
  Used Flash,1275860,5494

.. csv-table:: Dynamic memory stats
  :header: "", "Free Heap", "Increased by"

  On Bootup, 83220, 472
  Post Commissioning, 143804, -152

6.2.7 Configuring logging event buffer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Matter events serve as a historical record, stored in chronological order in the logging event buffer.
By reducing the buffer size we can potentially save the DRAM. However, it's important to note that this reduction
could lead to the omission of events.

For instance, reducing the critical log buffer from 4K to 1K could save 3K DRAM, but it comes with the trade-off of
potentially missing critical events.

::

    CONFIG_EVENT_LOGGING_CRIT_BUFFER_SIZE=1024

.. csv-table:: Static memory stats
  :header: "", "Size", "Decreased by"

  Used D/IRAM,158610,3072
  Used Flash,1275860,0

6.2.8 Excluding Unused Matter Clusters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the cluster implementation source files use a class derived from another class with virtual functions and instantiate
a global object of this class, the linker may keep all the related symbols that may be used for this class in the vtable.
To eliminate these symbols, you can deselect the unused Matter clusters under ``→ Component config`` → ``ESP Matter`` →
``Select Supported Matter Clusters``. Excluding unused clusters will help reduce flash and memory usage.

::

    CONFIG_SUPPORT_ACCOUNT_LOGIN_CLUSTER=n
    CONFIG_SUPPORT_ACTIVATED_CARBON_FILTER_MONITORING_CLUSTER=n
    CONFIG_SUPPORT_AIR_QUALITY_CLUSTER=n
    CONFIG_SUPPORT_APPLICATION_BASIC_CLUSTER=n
    CONFIG_SUPPORT_APPLICATION_LAUNCHER_CLUSTER=n
    CONFIG_SUPPORT_AUDIO_OUTPUT_CLUSTER=n
    CONFIG_SUPPORT_BOOLEAN_STATE_CONFIGURATION_CLUSTER=n
    CONFIG_SUPPORT_BRIDGED_DEVICE_BASIC_INFORMATION_CLUSTER=n
    CONFIG_SUPPORT_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_CARBON_MONOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_CHANNEL_CLUSTER=n
    CONFIG_SUPPORT_CHIME_CLUSTER=n
    CONFIG_SUPPORT_COMMISSIONER_CONTROL_CLUSTER=n
    CONFIG_SUPPORT_CONTENT_LAUNCHER_CLUSTER=n
    CONFIG_SUPPORT_CONTENT_CONTROL_CLUSTER=n
    CONFIG_SUPPORT_CONTENT_APP_OBSERVER_CLUSTER=n
    CONFIG_SUPPORT_DEVICE_ENERGY_MANAGEMENT_CLUSTER=n
    CONFIG_SUPPORT_DEVICE_ENERGY_MANAGEMENT_MODE_CLUSTER=n
    CONFIG_SUPPORT_DIAGNOSTIC_LOGS_CLUSTER=n
    CONFIG_SUPPORT_DISHWASHER_ALARM_CLUSTER=n
    CONFIG_SUPPORT_DISHWASHER_MODE_CLUSTER=n
    CONFIG_SUPPORT_MICROWAVE_OVEN_MODE_CLUSTER=n
    CONFIG_SUPPORT_DOOR_LOCK_CLUSTER=n
    CONFIG_SUPPORT_ECOSYSTEM_INFORMATION_CLUSTER=n
    CONFIG_SUPPORT_ELECTRICAL_ENERGY_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_ELECTRICAL_POWER_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_ENERGY_EVSE_CLUSTER=n
    CONFIG_SUPPORT_ENERGY_EVSE_MODE_CLUSTER=n
    CONFIG_SUPPORT_ENERGY_PREFERENCE_CLUSTER=n
    CONFIG_SUPPORT_FAN_CONTROL_CLUSTER=n
    CONFIG_SUPPORT_FAULT_INJECTION_CLUSTER=n
    CONFIG_SUPPORT_FIXED_LABEL_CLUSTER=n
    CONFIG_SUPPORT_FORMALDEHYDE_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_HEPA_FILTER_MONITORING_CLUSTER=n
    CONFIG_SUPPORT_ICD_MANAGEMENT_CLUSTER=n
    CONFIG_SUPPORT_KEYPAD_INPUT_CLUSTER=n
    CONFIG_SUPPORT_LAUNDRY_WASHER_MODE_CLUSTER=n
    CONFIG_SUPPORT_LOCALIZATION_CONFIGURATION_CLUSTER=n
    CONFIG_SUPPORT_LOW_POWER_CLUSTER=n
    CONFIG_SUPPORT_MEDIA_INPUT_CLUSTER=n
    CONFIG_SUPPORT_MEDIA_PLAYBACK_CLUSTER=n
    CONFIG_SUPPORT_MICROWAVE_OVEN_CONTROL_CLUSTER=n
    CONFIG_SUPPORT_MESSAGES_CLUSTER=n
    CONFIG_SUPPORT_MODE_SELECT_CLUSTER=n
    CONFIG_SUPPORT_NITROGEN_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_SAMPLE_MEI_CLUSTER=n
    CONFIG_SUPPORT_OCCUPANCY_SENSING_CLUSTER=n
    CONFIG_SUPPORT_POWER_TOPOLOGY_CLUSTER=n
    CONFIG_SUPPORT_OPERATIONAL_STATE_CLUSTER=n
    CONFIG_SUPPORT_OPERATIONAL_STATE_OVEN_CLUSTER=n
    CONFIG_SUPPORT_OPERATIONAL_STATE_RVC_CLUSTER=n
    CONFIG_SUPPORT_OVEN_MODE_CLUSTER=n
    CONFIG_SUPPORT_OZONE_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_PM10_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_PM1_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_PM2_5_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_POWER_SOURCE_CLUSTER=n
    CONFIG_SUPPORT_POWER_SOURCE_CONFIGURATION_CLUSTER=n
    CONFIG_SUPPORT_PUMP_CONFIGURATION_AND_CONTROL_CLUSTER=n
    CONFIG_SUPPORT_RADON_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_REFRIGERATOR_ALARM_CLUSTER=n
    CONFIG_SUPPORT_REFRIGERATOR_AND_TEMPERATURE_CONTROLLED_CABINET_MODE_CLUSTER=n
    CONFIG_SUPPORT_RVC_CLEAN_MODE_CLUSTER=n
    CONFIG_SUPPORT_RVC_RUN_MODE_CLUSTER=n
    CONFIG_SUPPORT_SERVICE_AREA_CLUSTER=n
    CONFIG_SUPPORT_SMOKE_CO_ALARM_CLUSTER=n
    CONFIG_SUPPORT_SOFTWARE_DIAGNOSTICS_CLUSTER=n
    CONFIG_SUPPORT_SWITCH_CLUSTER=n
    CONFIG_SUPPORT_TARGET_NAVIGATOR_CLUSTER=n
    CONFIG_SUPPORT_TEMPERATURE_CONTROL_CLUSTER=n
    CONFIG_SUPPORT_THERMOSTAT_CLUSTER=n
    CONFIG_SUPPORT_THERMOSTAT_USER_INTERFACE_CONFIGURATION_CLUSTER=n
    CONFIG_SUPPORT_THREAD_BORDER_ROUTER_MANAGEMENT_CLUSTER=n
    CONFIG_SUPPORT_THREAD_NETWORK_DIRECTORY_CLUSTER=n
    CONFIG_SUPPORT_TIME_FORMAT_LOCALIZATION_CLUSTER=n
    CONFIG_SUPPORT_TIME_SYNCHRONIZATION_CLUSTER=n
    CONFIG_SUPPORT_TIMER_CLUSTER=n
    CONFIG_SUPPORT_TVOC_CONCENTRATION_MEASUREMENT_CLUSTER=n
    CONFIG_SUPPORT_UNIT_TESTING_CLUSTER=n
    CONFIG_SUPPORT_USER_LABEL_CLUSTER=n
    CONFIG_SUPPORT_VALVE_CONFIGURATION_AND_CONTROL_CLUSTER=n
    CONFIG_SUPPORT_WAKE_ON_LAN_CLUSTER=n
    CONFIG_SUPPORT_LAUNDRY_WASHER_CONTROLS_CLUSTER=n
    CONFIG_SUPPORT_LAUNDRY_DRYER_CONTROLS_CLUSTER=n
    CONFIG_SUPPORT_WIFI_NETWORK_MANAGEMENT_CLUSTER=n
    CONFIG_SUPPORT_WINDOW_COVERING_CLUSTER=n
    CONFIG_SUPPORT_WATER_HEATER_MANAGEMENT_CLUSTER=n
    CONFIG_SUPPORT_WATER_HEATER_MODE_CLUSTER=n

.. csv-table:: Static memory stats
  :header: "", "Size", "Decreased by"

  Used D/IRAM,167706,1336
  Used Flash,1348790,21996

6.3 References for futher optimizations
---------------------------------------

- `RAM optimization`_
- `Binary size optimization`_
- `Speed Optimization`_
- `ESP32 Memory Analysis — Case Study`_
- `Optimizing IRAM`_ can provide additional heap area but at the cost of execution speed. Relocating frequently-called
  functions from IRAM to flash may result in increased execution time


.. _`v5.1.2`: https://github.com/espressif/esp-idf/tree/v5.1.2
.. _`cb3bc9d`: https://github.com/espressif/esp-matter/tree/cb3bc9d
.. _`light`: https://github.com/espressif/esp-matter/tree/cb3bc9d/examples/light
.. _`newlib nano formatting guide`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/size.html#newlib-nano-formatting
.. _`RAM optimization`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/ram-usage.html
.. _`Binary size optimization`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/size.html
.. _`Speed Optimization`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/speed.html
.. _`ESP32 Memory Analysis — Case Study`: https://blog.espressif.com/esp32-memory-analysis-case-study-eacc75fe5431
.. _`Optimizing IRAM`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/ram-usage.html#optimizing-iram-usage
