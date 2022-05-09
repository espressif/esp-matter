A1 Appendix FAQs
================

A1.1 Compilation errors
-----------------------

I cannot build the application:

-  Make sure you are on the correct esp-idf branch. Run ``git submodule
   update —init —recursive`` to make sure the submodules are at the
   correct heads
-  Make sure you have the correct ESP_MATTER_PATH (and any other paths)
   is (are) exported.
-  Delete the build/ directory and also sdkconfig and sdkconfig.old and
   then build again.
-  If you are still facing issues, reproduce the issue on the default
   example and then raise an `issue <https://github.com/espressif/esp-matter/issues>`__. Please make sure to
   share these:

   -  The complete build logs.
   -  The esp-matter and esp-idf branch you are using.

A1.2 Device commissioning using chip-tool
-----------------------------------------

I cannot commission a new device through the chip-tool:

-  If the ``chip-tool pairing ble-wifi`` command is failing,
   make sure the arguments are correct.
-  Make sure Bluetooth is turned on, on your client (host).
-  If you are still facing issues, reproduce the issue on the default
   example for the device and then raise an `issue <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs.
   -  The complete chip-tool logs.
   -  The esp-matter and esp-idf branch you are using.

Bluetooth/BLE does not work on by device:

-  There are some known issues where BLE does not work on MacOS.
-  In this case, the following can be done:

   -  Run the device console command:
      ``matter wifi connect <ssid> <password>``.
   -  Run the chip-tool command for commissioning over ip:
      ``chip-tool pairing onnetwork 1 20202021``.

-  If you are still facing issues, reproduce the issue on the default
   example for the device and then raise an `issue <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs taken over UART.
   -  The complete chip-tool logs.
   -  The esp-matter and esp-idf branch you are using.

A1.3 Device crashing
--------------------

My device is crashing:

-  Given the tight footprint requirements of the device, please make
   sure any issues in your code have been ruled out. If you believe the
   issue is with the ESP Matter SDK itself, please recreate the issue on
   the default example application (without any changes) and go through
   the following steps:
-  Make sure you are on the correct esp-idf branch. Run ``git submodule
   update —init —recursive`` to make sure the submodules are at the
   correct heads.
-  Make sure you have the correct ESP_MATTER_PATH (and any other paths)
   is (are) exported.
-  Delete the build/ directory and also sdkconfig and sdkconfig.old and
   then build and flash again.
-  If you are still facing issues, reproduce the issue on the default
   example for the device and then raise an `issue <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The steps you followed to reproduce the issue.

   -  The complete device logs taken over UART.

   -  

      .. raw:: html

         <example>

      .elf file from the build/ directory.

   -  If you have gdb enabled, run the command ``backtrace`` and share the
      output of gdb too.

   -  The esp-matter and esp-idf branch you are using.

A1.4 Device not crashed but not responding
------------------------------------------

My device is not responding to commands:

-  Make sure your device is commissioned successfully and connected is
   to the Wi-Fi.
-  Make sure the node_id and the endpoint_id are correct in the command
   from chip-tool.
-  If you are still facing issues, reproduce the issue on the default
   example for the device and then raise an `issue <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The steps you followed to reproduce the issue.
   -  The complete device logs taken over UART.
   -  The esp-matter and esp-idf branch you are using.

A1.5 QR code not rendering
--------------------------

The QR code on my device console is not rendering properly:

-  Check the below lines on your terminal and copy paste the given URL
   in a browser:

   ::

      If QR code is not visible, copy paste the URL in a browser:
      https://dhrishi.github.io/connectedhomeip/qrcode.html?data=....

A1.6 Onboard LED not working
----------------------------

The LED on my devkit is not working:

-  Make sure you have selected the proper ``device``. You can explicitly
   do that by exporting the ``ESP_MATTER_DEVICE_PATH`` to the correct
   path.
-  Check the version of your board, and if it has the LED connected to a
   different pin. If it is different, you can change the
   led_driver_config_t accordingly in the device.c file.
-  If you are still facing issues, reproduce the issue on the default
   example for the device and then raise an `issue <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs taken over UART.
   -  The esp-matter and esp-idf branch you are using.
   -  The devkit and its version that you are using.

**Also check the "Appendix FAQs" sections of the respective examples.**

.. toctree::
   :maxdepth: 1

   Light <examples/light>
   RainMaker Light <examples/rainmaker_light>
   Switch <examples/switch>
   Zap Light <examples/zap_light>
   Zigbee Bridge <examples/bridge_zigbee>
