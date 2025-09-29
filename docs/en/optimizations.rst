Configuration options to optimize RAM and Flash
===============================================

1 Overview
----------

There are several configuration options available to optimize Flash and RAM storage.
The following list highlights key options that significantly increase the free DRAM, heap, and reduce the flash
footprint.

For more optimizations, we've also listed the reference links to esp-idf's optimization guide.

2 Configurations
----------------

2.1 Test Environment setup
~~~~~~~~~~~~~~~~~~~~~~~~~~

All numbers mentioned below are collected in the following environment:

.. only:: esp32h2

    - esp-idf `v5.4.1`_
    - esp-matter `6a77422`_
    - Example: `light`_
    - SoC: ESP32-H2

.. only:: not esp32h2

    - esp-idf `v5.4.1`_
    - esp-matter `6a77422`_
    - Example: `light`_
    - SoC: ESP32-C3

.. note::

    - These numbers may vary slightly in a different environment.
    - All numbers are in bytes
    - As we are using BLE only for commissioning, BLE memory is freed post commissioning,
      hence there is an increase in the free heap post commissioning. (``CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING=y``)
    - After building an example, some DRAM will be utilized, and the remaining DRAM will be
      allocated as heap. Therefore, a direct increase in the free DRAM will reflect as an increase in free heap.


2.2 Default Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

We have used the default light example here, and below listed are the static and dynamic sizes.

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,179487,--
      Used Flash,1576436,--

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 44256, --
      Post Commissioning, 77976, --

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,195080,--
      Used Flash,1476960,--

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 35976, --
      Post Commissioning, 101580, --


2.3 Disable the chip-shell
~~~~~~~~~~~~~~~~~~~~~~~~~~

Console shell is helpful when developing/debugging the application, but may not be necessary in
production. Disabling the shell can save space. Disable the below configuration option.

::

    CONFIG_ENABLE_CHIP_SHELL=n

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,178695,792
      Used Flash,1521816,54620

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 54136, 9880
      Post Commissioning, 87592, 9616

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,192892,2188
      Used Flash,1410424,66536

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 46476, 10500
      Post Commissioning, 112340, 10760


2.4 Adjust the dynamic endpoint count
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The default dynamic endpoint count and default device type count is 16, which may be excessive for a normal application creating only 2 endpoints.
eg: light, only has two endpoints, one for root endpoint and one for actual light.
Adjusting this to a lower value, corresponding to the actual number of endpoints the application will create, can save DRAM.

Here, we have set the dynamic endpoint count and device type count to 2. Increase in the DRAM per endpoint/count is ~550 bytes.

::

    CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT=2
    CONFIG_ESP_MATTER_MAX_DEVICE_TYPE_COUNT=2

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,172859,6628
      Used Flash,1576048,388

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 51020, 6764
      Post Commissioning, 84208, 6232

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,188452,6628
      Used Flash,1476850,110

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 41932, 5956
      Post Commissioning, 107984, 6404


2.5 Use the newlib nano formatting
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This optimization saves approximately 25-50K of flash, depending on the target. In our case, it results in a flash
reduction of 47 KB.

Additionally, it lowers the high watermark of task stack for functions that call printf() or other string formatting
functions. Fore more details please take a look at esp-idf's `newlib nano formatting guide`_.

::

    CONFIG_NEWLIB_NANO_FORMAT=y

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,179487,0
      Used Flash,1529228,47208

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 46164, 1908
      Post Commissioning, 79616, 1640

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,195080,0
      Used Flash,1429916,47044

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 38404, 2428
      Post Commissioning, 103500, 1920


2.6 BLE Optimizations
~~~~~~~~~~~~~~~~~~~~~

Since most devices will primarily operate as BLE peripherals and typically won't need more than one connection
(especially if it's just a Matter app), we can optimize by reducing the maximum allowed connections, thereby
saving DRAM. Additionally, given the peripheral nature of these devices, we can disable the central and
observer roles, for further optimization.
In current implementation, BLE is disabled once commissioning succeeds, so these optimizations do not contribute to free heap post-commissioning.

Below are the configuration options that can be set to achieve these optimizations.

::

    CONFIG_NIMBLE_MAX_CONNECTIONS=1
    CONFIG_BTDM_CTRL_BLE_MAX_CONN=1
    CONFIG_BT_NIMBLE_MAX_CONNECTIONS=1
    CONFIG_BT_NIMBLE_ROLE_CENTRAL=n
    CONFIG_BT_NIMBLE_ROLE_OBSERVER=n
    CONFIG_BT_NIMBLE_MAX_BONDS=2
    CONFIG_BT_NIMBLE_MAX_CCCDS=2
    CONFIG_BT_NIMBLE_SECURITY_ENABLE=n
    CONFIG_BT_NIMBLE_50_FEATURE_SUPPORT=n
    CONFIG_BT_NIMBLE_WHITELIST_SIZE=1
    CONFIG_BT_NIMBLE_GATT_MAX_PROCS=1
    CONFIG_BT_NIMBLE_MSYS_1_BLOCK_COUNT=10
    CONFIG_BT_NIMBLE_MSYS_1_BLOCK_SIZE=100
    CONFIG_BT_NIMBLE_MSYS_2_BLOCK_COUNT=4
    CONFIG_BT_NIMBLE_MSYS_2_BLOCK_SIZE=320
    CONFIG_BT_NIMBLE_ACL_BUF_COUNT=5
    CONFIG_BT_NIMBLE_HCI_EVT_HI_BUF_COUNT=5
    CONFIG_BT_NIMBLE_HCI_EVT_LO_BUF_COUNT=3
    CONFIG_BT_NIMBLE_ENABLE_CONN_REATTEMPT=n

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,177753,1734
      Used Flash,1552372,24064

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 54096, 9840
      Post Commissioning, 77728, -248

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,192920,2160
      Used Flash,1454332,22628

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 55048, 19072
      Post Commissioning, 101176, -404


2.7 Configuring logging event buffer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Matter events serve as a historical record, stored in chronological order in the logging event buffer.
By reducing the buffer size we can potentially save the DRAM. However, it's important to note that this reduction
could lead to the omission of events.

For instance, reducing the critical log buffer from 4K to 256 bytes could save 3K+ DRAM, but it comes with the trade-off of
potentially missing critical events.

::

    CONFIG_EVENT_LOGGING_CRIT_BUFFER_SIZE=256
    CONFIG_EVENT_LOGGING_INFO_BUFFER_SIZE=256
    CONFIG_EVENT_LOGGING_DEBUG_BUFFER_SIZE=256
    CONFIG_MAX_EVENT_QUEUE_SIZE=20

Reduce ESP system event queue size and event task stack size can increase free heap size.

::

    CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE=16
    CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE=2048
    CONFIG_MAX_EVENT_QUEUE_SIZE=20

Reduce the chip device event queue size can reduce IRAM size usage, lead to free heap increase.

::

    CONFIG_MAX_EVENT_QUEUE_SIZE=20

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,174111,5376
      Used Flash,1576434,0

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 51288, 7032
      Post Commissioning, 84868, 6892

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,189704,5376
      Used Flash,1477100,-140

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 42504, 6528
      Post Commissioning, 108184, 6604


2.8 Relocate certain code from IRAM to flash memory
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Relocating certain code from IRAM to flash can reduce IRAM usage, so increase available heap size. However, this may increase execution time.

.. note::

    The options in this section may impact performance. Please perform thorough testing before using them in production.

2.8.1 Reduce BLE IRAM usage
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Move most IRAM into flash. This will increase the usage of flash and reduce ble performance.
Because the code is moved to the flash, the execution speed of the code is reduced. To have
a small impact on performance, you need to enable flash suspend (SPI_FLASH_AUTO_SUSPEND).

::

    CONFIG_BT_CTRL_RUN_IN_FLASH_ONLY=y

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,159553,19934
      Used Flash,1589720,-13284

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 64044, 19788
      Post Commissioning, 97608, 19632

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,175718,19362
      Used Flash,1619786,-142826

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 59056, 23080
      Post Commissioning, 119608, 18028


2.8.2 Place FreeRTOS functions into Flash
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When enabled the selected Non-ISR FreeRTOS functions will be placed into Flash memory instead of IRAM.
This saves up to 8KB of IRAM depending on which functions are used.

::

    CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH=y

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,170409,9078
      Used Flash,1585754,-9318

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 53344, 9088
      Post Commissioning, 86780, 8804

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,184754,10326
      Used Flash,1487608,-10648

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 45432, 9456
      Post Commissioning, 111020, 9440


2.8.3 Place non-ISR ringbuf functions into flash
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Place non-ISR ringbuf functions (like xRingbufferCreate/xRingbufferSend) into flash.
This frees up IRAM, but the functions can no longer be called when the cache is disabled.

::

    CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH=y

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,174741,4746
      Used Flash,1581604,-5168

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 48860, 4604
      Post Commissioning, 82444, 4468

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,190334,4746
      Used Flash,1482260,-5300

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 39928, 3952
      Post Commissioning, 105652, 4072


2.8.4 Use esp_flash implementation in ROM
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Enable this flag to use new SPI flash driver functions from ROM instead of ESP-IDF.
After enable CONFIG_SPI_FLASH_ROM_IMPL, will increase free IRAM.
But may miss out on some flash features and support for new flash chips.

::

    CONFIG_SPI_FLASH_ROM_IMPL=y
    CONFIG_SPI_MASTER_ISR_IN_IRAM=n
    CONFIG_SPI_SLAVE_ISR_IN_IRAM=n

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,166798,12689
      Used Flash,1573452,2984

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 56900, 12644
      Post Commissioning, 90204, 12228

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,185590,9490
      Used Flash,1474292,2668

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 44316, 8340
      Post Commissioning, 110512, 8932


2.8.5 Force the entire heap component to be placed in flash memory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Enable this flag to save up RAM space by placing the heap component in the flash memory
Note that it is only safe to enable this configuration if no functions from esp_heap_caps.h or
esp_heap_trace.h are called from IRAM ISR which runs when cache is disabled.

::

    CONFIG_HEAP_PLACE_FUNCTION_INTO_FLASH=y

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,179487,0
      Used Flash,1576436,0

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 44124, -132
      Post Commissioning, 77564, -412

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,187936,7144
      Used Flash,1441086,-7218

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 42500, 6524
      Post Commissioning, 108192, 6612


2.9 Reduce Task Stack Size
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Reduce some task stack size can increase free heap size.

::

    CONFIG_ESP_MAIN_TASK_STACK_SIZE=3072
    CONFIG_ESP_TIMER_TASK_STACK_SIZE=2048
    CONFIG_CHIP_TASK_STACK_SIZE=6144

.. only:: esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,179487,0
      Used Flash,1576448,0

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 48204, 3948
      Post Commissioning, 81660, 3684

.. only:: not esp32h2

    .. csv-table:: Static memory stats
      :header: "", "Size", "Decreased by"

      Used D/IRAM,195080,0
      Used Flash,1477114,-154

    .. csv-table:: Dynamic memory stats
      :header: "", "Free Heap", "Increased by"

      On Bootup, 39304, 3328
      Post Commissioning, 104828, 3248


2.10 Excluding Unused Matter Clusters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the cluster implementation source files use a class derived from another class with virtual functions and instantiate
a global object of this class, the linker may keep all the related symbols that may be used for this class in the vtable.
To eliminate these symbols, you can deselect the unused Matter clusters under ``→ Component config`` → ``ESP Matter`` →
``Select Supported Matter Clusters``. Excluding unused clusters will help reduce flash and memory usage.
The default configuration disables all unused clusters.

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

  Used D/IRAM,179487,3736
  Used Flash,1576436,36938

.. csv-table:: Dynamic memory stats
  :header: "", "Free Heap", "Increased by"

  On Bootup, 44256, 3876
  Post Commissioning, 77976, 4164


.. only:: esp32c2 or esp32c3 or esp32c5 or esp32c6 or esp32h2

    2.11 Link Time Optimization (LTO)
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Link Time Optimization (LTO) helps further optimize both binary size and runtime performance.
    You can read more about LTO in `GCC's LTO documentation`_.

    For details on enabling LTO in ESP-IDF, along with its effects and known
    limitations, please refer to `ESP-IoT-Solution's LTO documentation`_.

    As demonstrated in the `example`_ listed in `ESP-IoT-Solution's LTO documentation`_,
    enabling LTO can result in around ~90 KB of flash savings, though it also increases stack usage by ~1700 bytes.


3 References for futher optimizations
-------------------------------------

- `RAM optimization`_
- `Binary size optimization`_
- `Speed Optimization`_
- `ESP32 Memory Analysis — Case Study`_
- `Optimizing IRAM`_ can provide additional heap area but at the cost of execution speed. Relocating frequently-called
  functions from IRAM to flash may result in increased execution time


.. _`v5.4.1`: https://github.com/espressif/esp-idf/tree/v5.4.1
.. _`6a77422`: https://github.com/espressif/esp-matter/tree/6a77422
.. _`light`: https://github.com/espressif/esp-matter/tree/6a77422/examples/light
.. _`newlib nano formatting guide`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/size.html#newlib-nano-formatting
.. _`RAM optimization`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/ram-usage.html
.. _`Binary size optimization`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/size.html
.. _`Speed Optimization`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/speed.html
.. _`ESP32 Memory Analysis — Case Study`: https://blog.espressif.com/esp32-memory-analysis-case-study-eacc75fe5431
.. _`Optimizing IRAM`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/performance/ram-usage.html#optimizing-iram-usage
.. _`GCC's LTO documentation`: https://gcc.gnu.org/onlinedocs/gccint/LTO.html
.. _`ESP-IoT-Solution's LTO documentation`: https://github.com/espressif/esp-iot-solution/blob/master/tools/cmake_utilities/docs/gcc.md
.. _`example`: https://github.com/espressif/esp-iot-solution/blob/master/tools/cmake_utilities/docs/gcc.md#example
