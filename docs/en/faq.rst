A1 Appendix FAQs
================

A1.1 Compilation errors
-----------------------

I cannot build the application:

-  Make sure you are on the correct esp-idf branch/release. Run ``git submodule
   update —init —recursive`` to make sure the submodules are at the
   correct heads
-  Make sure you have the correct ESP_MATTER_PATH (and any other required paths).
-  Delete the build/ directory and also sdkconfig and sdkconfig.old and
   then build again.
-  If you are still facing issues, reproduce it on the default
   example and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Please make sure to
   share these:

   -  The complete device logs taken over UART.
   -  The esp-matter and esp-idf branch you are using.

A1.2 Device commissioning using chip-tool
-----------------------------------------

I cannot commission a new device through the chip-tool:

-  If the ``chip-tool pairing ble-wifi`` command is failing,
   make sure the arguments are correct.
-  Make sure Bluetooth is turned on, on your client (host).
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs.
   -  The complete chip-tool logs.
   -  The esp-matter and esp-idf branch you are using.

Bluetooth/BLE does not work on by device:

-  There are some `known issues <https://github.com/project-chip/connectedhomeip/issues/13303>`__ where BLE does not work on MacOS.
-  In this case, the following can be done:

   -  Run the device console command:
      ``matter wifi connect <ssid> <password>``.
   -  Run the chip-tool command for commissioning over ip:
      ``chip-tool pairing onnetwork 0x7283 20202021``.

-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs taken over UART.
   -  The complete chip-tool logs.
   -  The esp-matter and esp-idf branch you are using.

A1.3 Device crashing
--------------------

My device is crashing:

-  Given the tight footprint requirements of the device, please make
   sure any issues in your code have been ruled out. If you believe the
   issue is with the Espressif SDK itself, please recreate the issue on
   the default example application (without any changes) and go through
   the following steps:
-  Make sure you are on the correct esp-idf branch. Run ``git submodule
   update —init —recursive`` to make sure the submodules are at the
   correct heads.
-  Make sure you have the correct ESP_MATTER_PATH (and any other paths)
   is (are) exported.
-  Delete the build/ directory and also sdkconfig and sdkconfig.old and
   then build and flash again.
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
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

-  Make sure your device is commissioned successfully and is connected
   to the Wi-Fi.
-  Make sure the node_id and the endpoint_id are correct in the command
   from chip-tool.
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
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
-  If you are still facing issues, reproduce it on the default
   example for the device and then raise it `here <https://github.com/espressif/esp-matter/issues>`__. Make sure
   to share these:

   -  The complete device logs taken over UART.
   -  The esp-matter and esp-idf branch you are using.
   -  The devkit and its version that you are using.

A1.7 Using Rotating Device Identifier
-------------------------------------

What is Rotating Device Identifier:

-  The Rotating Device Identifier provides a non-trackable identifier which is unique per-device and
   that can be used in one or more of the following ways:

   -  Provided to the vendor's customer support for help in pairing or establishing Node provenance;
   -  Used programmatically to obtain a Node's Passcode or other information in order to provide a
      simplified setup flow. Note that the mechanism by which the Passcode may be obtained is
      outside of this specification. If the Rotating Device Identifier is to be used for this purpose, the
      system implementing this feature SHALL require proof of possession by the user at least once
      before providing the Passcode. The mechanism for this proof of possession, and validation of it,
      is outside of this specification.

How to use Rotating Device Identifier

-  Enable the Rotating Device Identifier support in menuconfig.
-  Add the ``--enable-rotating-device-id`` and add the ``--rd-id-uid`` to specify the ``Rotating ID Unique ID``
   when use the mfg_tool.py to generate partition.bin file.

Difference between Rotating ID Unique ID and Unique ID

-  The ``Rotating ID Unique ID`` is a parameter used to generate ``Rotating Device Identifier``, it is
   a unique per-device identifier and shall consist of a randomly-generated 128-bit or longer octet string which
   shall be programmed during factory provisioning or delivered to the device by the vendor using secure means
   after a software update, it shall stay fixed during the lifetime of the device.

-  The ``Unique ID`` is an attribute in ``Basic Information Cluster``, it shall indicate a unique
   identifier for the device, which is constructed in a manufacturer specific manner. It may be
   constructed using a permanent device identifier (such as device MAC address) as basis.
   In order to prevent tracking:

   -  it SHOULD NOT be identical to (or easily derived from) such permanent device identifier
   -  it SHOULD be updated when the device is factory reset
   -  it SHALL not be identical to the SerialNumber attribute
   -  it SHALL not be printed on the product or delivered with the product

   The value does not need to be human readable.

**Also check the "Appendix FAQs" sections of the respective examples.**

-  :project_file:`Light <examples/light/README.md>`
-  :project_file:`Light Switch <examples/light_switch/README.md>`
-  :project_file:`Zap Light <examples/zap_light/README.md>`
-  :project_file:`Zigbee Bridge <examples/zigbee_bridge/README.md>`
-  :project_file:`BLE Mesh Bridge <examples/blemesh_bridge/README.md>`
